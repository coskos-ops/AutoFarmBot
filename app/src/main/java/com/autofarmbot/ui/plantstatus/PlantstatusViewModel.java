package com.autofarmbot.ui.plantstatus;

import android.widget.TableLayout;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

public class PlantstatusViewModel extends ViewModel {

    private final MutableLiveData<TableLayout> table;


    public PlantstatusViewModel() {
        table = new MutableLiveData<>();
    }

}