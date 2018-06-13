//
// Created by Shawn Roach on 1/14/2018.
//

#ifndef PLAYGROUND_UTCALLBACKLISTN_HPP
#define PLAYGROUND_UTCALLBACKLISTN_HPP

#include "UtCallbackList.hpp"

template<typename SIGNATURE, typename EnumerationType>
class CallbackListN;

template<typename R, typename... CB_ARGS, typename EnumerationType>
class CallbackListN<R(CB_ARGS...), EnumerationType> : public CallbackList<R(CB_ARGS...)> {
public:
    using base_cb_list_t = CallbackList<R(CB_ARGS...)>;

    bool Empty() const override { return base_cb_list_t::Empty() && mEnumMap.empty(); }

protected:
    class CbListEnumeration;

public:
    //todo document operator[]
    [[nodiscard]] CbListEnumeration& operator[](const EnumerationType& aEnum) {
        // inserts a new list into the mEnumMap (with proper constructor args) if it does not exist, and return that enumeration
        return (mEnumMap.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(aEnum),
                                 std::forward_as_tuple(*this, aEnum)).first->second);
    }

protected:
    class CbListEnumeration : public base_cb_list_t {
    public:
        CbListEnumeration(CallbackListN<R(CB_ARGS...), EnumerationType>& mGlobalList, const EnumerationType mEnumVal) : mGlobalList(
                mGlobalList), mEnumVal(mEnumVal) {}

        void operator()(CB_ARGS... aArgs) override {
            //called functions stored in this enumeration
            base_cb_list_t::operator()(aArgs...);
            //call function stored to be called no matter what the enumeration is.
            mGlobalList.basicCall(aArgs...);
            //prevent enumerations with nothing to ever be called in them from building up in the EnumMap
            if ((*this).Empty()) {
                mGlobalList.removeEnumeration(mEnumVal);
            }
        }
    protected:
        void disconnect(std::size_t index) override {
            base_cb_list_t::disconnect(index);
            if ((*this).Empty()) {
                mGlobalList.removeEnumeration(mEnumVal);
            }
        };

        void disconnectAll() override {
            base_cb_list_t::disconnectAll();
            mGlobalList.removeEnumeration(mEnumVal);
        };

    private:
        CallbackListN<R(CB_ARGS...), EnumerationType>& mGlobalList;
        const EnumerationType mEnumVal;

        friend class CallbackListN<R(CB_ARGS...), EnumerationType>;
    };

    void removeEnumeration(const EnumerationType& aEnum) { mEnumMap.erase(aEnum); }

    void disconnectAll() override {
        base_cb_list_t::disconnectAll();
        mEnumMap.clear();
    };

    std::unordered_map<EnumerationType, CbListEnumeration> mEnumMap;
};

#endif //PLAYGROUND_UTCALLBACKLISTN_HPP
