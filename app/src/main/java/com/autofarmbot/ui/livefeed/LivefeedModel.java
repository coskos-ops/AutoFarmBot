package com.autofarmbot.ui.livefeed;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

public class LivefeedModel extends ViewModel {

    private final MutableLiveData<String> mText;

    public LivefeedModel() {
        mText = new MutableLiveData<>();
        mText.setValue("This is notifications fragment");
    }

    public LiveData<String> getText() {
        return mText;
    }
}