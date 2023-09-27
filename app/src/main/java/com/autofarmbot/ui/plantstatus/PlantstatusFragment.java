package com.autofarmbot.ui.plantstatus;

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

    private FragmentPlantstatusBinding binding;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        PlantstatusViewModel plantstatusViewModel =
                new ViewModelProvider(this).get(PlantstatusViewModel.class);

        binding = FragmentPlantstatusBinding.inflate(inflater, container, false);
        View root = binding.getRoot();

        final TableLayout textView = binding.table;

        return root;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }
}