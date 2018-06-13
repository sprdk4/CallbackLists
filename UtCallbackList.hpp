//
// Created by Shawn Roach on 1/14/2018.
//

#ifndef PLAYGROUND_UTCALLBACKLIST_HPP
#define PLAYGROUND_UTCALLBACKLIST_HPP


#include <vector>
#include <functional>

//todo account for callbacks disconnecting other callbacks without having traversal of callbacks being called invalidated
class CallbackListInterface {
public:
    CallbackListInterface() = default;

    virtual ~CallbackListInterface() { disconnectAll(); }

    virtual bool Empty() const { return mCallbacks.empty(); }

    class CallbackInterface {
    public:
        CallbackInterface() = default;

        CallbackInterface(const CallbackInterface&) = delete;

        CallbackInterface(CallbackInterface&& aRhs)
                : mListConnectedTo(aRhs.mListConnectedTo), mConnectedFunctionIndex(aRhs.mConnectedFunctionIndex) {
            aRhs.mListConnectedTo = nullptr;
            mListConnectedTo->mCallbacks[mConnectedFunctionIndex] = this;
        }

        virtual ~CallbackInterface() {
            if (mListConnectedTo != nullptr) {
                mListConnectedTo->disconnect(mConnectedFunctionIndex);
            }
        }

        explicit CallbackInterface(CallbackListInterface* aListConnectingTo, std::size_t aIndexOfCallbackFunction)
                : mListConnectedTo(aListConnectingTo), mConnectedFunctionIndex(aIndexOfCallbackFunction) {
            aListConnectingTo->mCallbacks.emplace_back(this);
        }

    private:
        CallbackListInterface* mListConnectedTo{nullptr};
        std::size_t mConnectedFunctionIndex;

        friend class CallbackListInterface;
    };

protected:
    virtual void disconnect(std::size_t index) {
        mCallbacks[index]->mListConnectedTo = nullptr;
        mCallbacks[index] = std::move(mCallbacks.back());
        mCallbacks[index]->mConnectedFunctionIndex = index;
        mCallbacks.pop_back();
    }

    virtual void disconnectAll() {
        for (auto* cb:mCallbacks) {
            cb->mListConnectedTo = nullptr;
        }
        mCallbacks.clear();
    };

    std::vector<CallbackInterface*> mCallbacks;
};

using CallbackInterface=CallbackListInterface::CallbackInterface;

//todo document CallbackHolder
class CallbackHolder : public std::vector<CallbackInterface> {
public:
    auto emplace(CallbackInterface&& aCallbackPtr)
    -> decltype(emplace_back(std::move(aCallbackPtr))) {
        return emplace_back(std::move(aCallbackPtr));
    }
};

template<typename SIGNATURE>
class CallbackList;

template<typename R, typename... CB_ARGS>
class CallbackList<R(CB_ARGS...)> : public CallbackListInterface {
public:
    CallbackList() = default;

    using callback_function_t = std::function<R(CB_ARGS...)>;

    /**
    * @brief calls every connected callback function with the provided arguements
    * @note if during this call a callback function in this list removes other callbacks within the same list, it is undefined if the
    *  removed callbacks will also be called.
    * @param aArgs arguements to call each callback function with
    */
    virtual void operator()(CB_ARGS... aArgs) {
        basicCall(aArgs...);
    }

    /// @brief Connects a function which will be called every time this list is called, until the
    /// returned smart ptr dies.
    /// @param aFunctor the functor you wish to called whenever this callback list is called.
    /// @return unique_ptr to newly connected callback, should be added to a callback holder with the same desired
    /// lifespan as the connected function. The only way to remove a connected functor is through the destruction
    /// of this returned unique_ptr
    template<typename Functor>
    [[nodiscard]]  std::enable_if_t<std::is_constructible<callback_function_t, Functor>::value,
            CallbackInterface> connect(Functor&& aFunctor) {
        mFunctions.emplace_back(std::forward<Functor>(aFunctor));
        return CallbackInterface{this, mFunctions.size() - 1};
    }

    /// @brief Connects a member function of the provided object instance to be called every time
    /// this callback list is called, as long as the returned smart ptr hasn't deleted itself.
    /// @param aObject the object that the member function will be called on
    /// @param aMemberFunc the member function belonging to the provided object
    /// @return unique_ptr to newly connected callback, should be added to a callback holder.
    template<typename T, typename CONVERTIBLE_R, typename... CONVERTIBLE_CB_ARGS>
    [[nodiscard]] CallbackInterface connect(T* aObject, CONVERTIBLE_R(T::*aMemberFunc)(CONVERTIBLE_CB_ARGS...)) {
        return connect([=](CONVERTIBLE_CB_ARGS... aArgs) -> CONVERTIBLE_R { return (aObject->*aMemberFunc)(aArgs...); });
    };

protected:
    std::vector<callback_function_t> mFunctions;

    //todo if possible, when a callback removes a different callback gaurentee said callback will be called before it is removes
    void disconnect(std::size_t index) override {
        CallbackListInterface::disconnect(index);
        mFunctions[index] = std::move(mFunctions.back());
        mFunctions.pop_back();
    }

    void disconnectAll() override {
        CallbackListInterface::disconnectAll();
        mFunctions.clear();
    }

    void basicCall(CB_ARGS... aArgs) {
        for (auto& func: mFunctions) {
            func(aArgs...);
        }
    }
};

#endif //PLAYGROUND_UTCALLBACKLIST_HPP
