package com.autofarmbot.ui.plantstatus;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TableLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.autofarmbot.databinding.FragmentPlantstatusBinding;

public class PlantstatusFragment extends Fragment {

    private static FragmentPlantstatusBinding binding;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        PlantstatusViewModel plantstatusViewModel =
                new ViewModelProvider(this).get(PlantstatusViewModel.class);

        binding = FragmentPlantstatusBinding.inflate(inflater, container, false);


        return binding.getRoot();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }

    /**
     * Changes the pH value displayed on the screen. The value will be green if it is within
     * the acceptable value defined in the specifications [6 - 7].
     * @param newValue the new pH value
     */
    @SuppressLint("SetTextI18n")
    public static void changePHValue(float newValue) {
        String strValue = String.format("%.2f", newValue);
        TextView phValue = binding.pHValue;
        phValue.setText(strValue);
        if(newValue < 6 || newValue > 7) {
            phValue.setTextColor(Color.RED);
        }
        else {
            phValue.setTextColor(Color.GREEN);
        }

    }

    /**
     * Changes the EC value displayed on the screen. The value will be green if it is within
     * the acceptable value defined in the specifications [1.4 - 1.8].
     * @param newValue the new EC value
     */
    @SuppressLint("SetTextI18n")
    public static void changeECValue(float newValue) {
        String strValue = String.format("%.2f", newValue);
        TextView ecValue = binding.ECvalue;
        binding.ECvalue.setText(strValue);
        if(newValue < 1.4 || newValue > 1.8) {
            ecValue.setTextColor(Color.RED);
        }
        else {
            ecValue.setTextColor(Color.GREEN);
        }

    }
}