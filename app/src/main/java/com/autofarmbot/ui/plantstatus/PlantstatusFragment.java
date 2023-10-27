package com.autofarmbot.ui.plantstatus;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TableLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.autofarmbot.databinding.FragmentPlantstatusBinding;
import com.autofarmbot.plant.Plant;
import com.autofarmbot.plant.PlantStatus;

/**
 * Plant status window
 */
public class PlantstatusFragment extends Fragment {


    /**
     * The binding used to alias GUI objects belonging to this window
     */
    private static FragmentPlantstatusBinding binding;

    /**
     * Color for data when it is in an acceptable range
     */
    private static final int GREEN = Color.rgb(76, 175, 80);
    /**
     * Color for data when it is in an unacceptable range
     */
    private static final int RED = Color.RED;
    /**
     * Minimum acceptable EC value (inclusive)
     */
    private static final float ECMINBOUND = 1.4F;
    /**
     * Maximum acceptable EC value (inclusive)
     */
    private static final float ECMAXBOUND = 1.8001F;
    /**
     * Minimum acceptable EC value (inclusive)
     */
    private static final float PHMINBOUND = 6;

    /**
     * Maximum acceptable EC value (inclusive)
     */
    private static final float PHMAXBOUND = 7;

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
     * the acceptable value defined in the specifications [PHMINBOUND - PHMAXBOUND].
     * @param newValue the new pH value
     */
    @SuppressLint("SetTextI18n")
    public static void changePHValue(float newValue) {
        String strValue = String.format("%.2f", newValue);
        TextView phValue = binding.pHValue;
        phValue.setText(strValue);
        if(newValue < PHMINBOUND || newValue > PHMAXBOUND) {
            phValue.setTextColor(RED);
        }
        else {
            phValue.setTextColor(GREEN);
        }

    }


    /**
     * Changes the distance Value
     * @param newValue the new distanceValue
     */
    @SuppressLint("SetTextI18n")
    public static void changeDistanceValue(int newValue) {
        String strValue = String.format("%d", newValue);
        TextView distanceValue = binding.distanceValue;
        distanceValue.setText(strValue);


    }


    /**
     * Changes the EC value displayed on the screen. The value will be green if it is within
     * the acceptable value defined in the specifications [ECMINBOUND - ECMAXBOUND].
     * @param newValue the new EC value
     */
    @SuppressLint("SetTextI18n")
    public static void changeECValue(float newValue) {
        String strValue = String.format("%.2f", newValue);
        TextView ecValue = binding.ECvalue;
        binding.ECvalue.setText(strValue);

        if(newValue < ECMINBOUND || newValue > ECMAXBOUND) {
            ecValue.setTextColor(RED);
        }
        else {
            ecValue.setTextColor(GREEN);
        }
    }

    /**
     * Change the status of a plant displayed in the plant view
     * @param p The plant to update
     */
    public static void changePlantStatus(Plant p) {
        String name = p.getName();
        TextView label;
        Button harvestButton;
        if(name.equals("P1")) {
            label = binding.P1status;
            harvestButton = binding.P1Harvest;
        }
        else if(name.equals("P2")) {
            label = binding.P2status;
            harvestButton = binding.P2Harvest;
        }
        else if(name.equals("P3")) {
            label = binding.P3status;
            harvestButton = binding.P3Harvest;
        }
        else if(name.equals("P4")) {
            label = binding.P4status;
            harvestButton = binding.P4Harvest;
        }
        else {
            throw new RuntimeException("Error! check how this is possible!");
        }

        PlantStatus newStatus = p.getStatus();
        switch (newStatus) {
            case EMPTY:
                label.setText("Empty");
                harvestButton.setEnabled(false);
                break;
            case GROWING:
                label.setText("Growing");
                harvestButton.setEnabled(false);
                break;
            case RIPE:
                label.setText("Ripe");
                harvestButton.setEnabled(true);
                break;
            default:
                throw new RuntimeException("Error! check how this is possible!");
        }
    }
}