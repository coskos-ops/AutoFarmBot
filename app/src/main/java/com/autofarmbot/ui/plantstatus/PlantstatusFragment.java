package com.autofarmbot.ui.plantstatus;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.autofarmbot.MainActivity;
import com.autofarmbot.databinding.FragmentPlantstatusBinding;
import com.autofarmbot.plant.Plant;
import com.autofarmbot.plant.PlantStatus;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.UUID;

/**
 * Plant status window
 */
public class PlantstatusFragment extends Fragment {


    /**
     * The binding used to alias GUI objects belonging to this window
     */
    private static FragmentPlantstatusBinding binding;

    private static int growthThreshold = 12;

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
    public static final float ECMINBOUND = 1.4F;
    /**
     * Maximum acceptable EC value (inclusive)
     */
    public static final float ECMAXBOUND = 1.8001F;
    /**
     * Minimum acceptable EC value (inclusive)
     */
    public static final float PHMINBOUND = 6;

    /**
     * Maximum acceptable EC value (inclusive)
     */
    public static final float PHMAXBOUND = 7;

    public static final int LOW_WATER_LEVEL = 50;
    public static final int CRITICAL_WATER_LEVEL = 0;

    /**
     * Minimum acceptable distance value (inclusive)
     */
    private static final int DISTMINBOUND = 40;

    /**
     * Maximum acceptable distance value (inclusive)
     */
    private static final int DISTMAXBOUND = 66;

    private int generateUniqueIntId() {
        // Generate a random UUID
        UUID uuid = UUID.randomUUID();

        // Convert UUID to long
        long uuidAsLong = uuid.getMostSignificantBits();

        // Use hash code to convert to int
        int uniqueId = (int) (uuidAsLong ^ (uuidAsLong >>> 32));

        // Ensure the ID is positive
        return Math.abs(uniqueId) % 100 + 1;
    }

    private class PostRequestTask extends AsyncTask<String, Void, Void> {

        private final PlantstatusFragment fragment;

        PostRequestTask(PlantstatusFragment fragment) {
            this.fragment = fragment;
        }

        @Override
        protected Void doInBackground(String... params) {
            try {
                // Extract parameters
                String postUrl = params[0];
                String jsonBody = params[1];

                // Create a URL object
                URL url = new URL(postUrl);

                // Open connection
                HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();

                try {
                    // Set the request method to POST
                    urlConnection.setRequestMethod("POST");

                    // Set the content type to application/json
                    urlConnection.setRequestProperty("Content-Type", "application/json");

                    // Enable input/output streams
                    urlConnection.setDoOutput(true);

                    // Write the JSON data to the output stream
                    OutputStream outputStream = urlConnection.getOutputStream();
                    BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(outputStream, "UTF-8"));
                    writer.write(jsonBody);
                    writer.flush();
                    writer.close();
                    outputStream.close();

                    // Get the response code (optional)
                    int responseCode = urlConnection.getResponseCode();

                    if(responseCode == 200) {
//                        fragment.requireActivity().runOnUiThread(() -> {
//                            binding.P1Harvest.setEnabled(false);
//                            binding.P2Harvest.setEnabled(false);
//                            binding.P3Harvest.setEnabled(false);
//                            binding.P4Harvest.setEnabled(false);
//                            binding.P5Harvest.setEnabled(false);
//                            binding.P6Harvest.setEnabled(false);
//
//                        });

                        // Get the commandId from the jsonBody
                        int commandId = extractCommandId(jsonBody);

                        // Loop until you get a 404 response
                        while (true) {
                            // Make a GET request to check command status
                            String checkUrl = postUrl + "/checkCommandStatus/" + commandId;
                            HttpURLConnection getStatusConnection = (HttpURLConnection) new URL(checkUrl).openConnection();
                            getStatusConnection.setRequestMethod("GET");

                            int getStatusResponseCode = getStatusConnection.getResponseCode();

                            if (getStatusResponseCode == 404) {
                                // Command not found, break the loop
                                break;
                            }

                            // Optionally, you can add a delay to avoid continuous polling and reduce server load
                            try {
                                Thread.sleep(1000); // Sleep for 1 second
                            }
                            catch (InterruptedException e) {

                            }
                        }
//                        fragment.requireActivity().runOnUiThread(() -> {
//                        binding.P1Harvest.setEnabled(true);
//                        binding.P2Harvest.setEnabled(true);
//                        binding.P3Harvest.setEnabled(true);
//                        binding.P4Harvest.setEnabled(true);
//                        binding.P5Harvest.setEnabled(true);
//                        binding.P6Harvest.setEnabled(true);
//
//                        });

                    }


                } finally {
                    // Disconnect the connection
                    urlConnection.disconnect();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }

        private int extractCommandId(String jsonBody) {
            try {
                JSONObject jsonObject = new JSONObject(jsonBody);
                return jsonObject.getInt("id");
            } catch (JSONException e) {
                e.printStackTrace();
            }
            return -1; // Return a default value or handle the error accordingly
        }

        // You can override onPostExecute if you need to handle the result
        // protected void onPostExecute(Void result) {
        //     // Handle the result
        // }
    }


    private void sendRequest(int id, int target, String action) {
        try {
            // Construct your JSON data
            JSONObject jsonData = new JSONObject();
            jsonData.put("id", id);
            jsonData.put("target", target);
            jsonData.put("action", action);

            // Convert the JSON data to a string
            String jsonBody = jsonData.toString();

            // Specify the URL where you want to send the POST request
            String postUrl = "https://coskos.pythonanywhere.com/postCommand"; // Replace this with your actual URL

            new PostRequestTask(this).execute(postUrl, jsonBody);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }



    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        PlantstatusViewModel plantstatusViewModel =
                new ViewModelProvider(this).get(PlantstatusViewModel.class);

        binding = FragmentPlantstatusBinding.inflate(inflater, container, false);

        binding.P1Harvest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int id = generateUniqueIntId();
                int target = 1;
                String action = "Harvest";
                sendRequest(id, target, action);
            }
        });

        binding.P2Harvest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int id = generateUniqueIntId();
                int target = 2;
                String action = "Harvest";
                sendRequest(id, target, action);
            }
        });

        binding.P3Harvest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int id = generateUniqueIntId();
                int target = 3;
                String action = "Harvest";
                sendRequest(id, target, action);
            }
        });

        binding.P4Harvest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int id = generateUniqueIntId();
                int target = 4;
                String action = "Harvest";
                sendRequest(id, target, action);
            }
        });

        binding.P5Harvest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int id = generateUniqueIntId();
                int target = 5;
                String action = "Harvest";
                sendRequest(id, target, action);
            }
        });

        binding.P6Harvest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int id = generateUniqueIntId();
                int target = 6;
                String action = "Harvest";
                sendRequest(id, target, action);
            }
        });

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
    public static void changePHValue(Double newValue) {
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
    public static void changeTempValue(Double newValue) {
        TextView tempValue = binding.TempValue;
        String strValue = String.format("%.2f", newValue);
        tempValue.setText(strValue);

    }



    public static void changeTDSValue(Double newValue) {
        TextView TDSValue = binding.TDSValue;
        String strValue = String.format("%.2f", newValue);
        TDSValue.setText(strValue);
    }

    public static void changeD1Value(Integer newValue) {
        TextView D1value = binding.P1status;
        String strValue = String.format("%d", newValue) + " cm";
        D1value.setText(strValue);
        if(newValue >= growthThreshold) {
            binding.P1Harvest.setEnabled(true);
        }
        else {
            binding.P1Harvest.setEnabled(false);
        }
    }

    public static void changeD2Value(Integer newValue) {
        TextView D2value = binding.P2status;
        String strValue = String.format("%d", newValue)  + " cm";
        D2value.setText(strValue);
        if(newValue >= growthThreshold) {
            binding.P2Harvest.setEnabled(true);
        }
        else {
            binding.P2Harvest.setEnabled(false);
        }
    }

    public static void changeD3Value(Integer newValue) {
        TextView D3value = binding.P3status;
        String strValue = String.format("%d", newValue)  + " cm";
        D3value.setText(strValue);
        if(newValue >= growthThreshold) {
            binding.P3Harvest.setEnabled(true);
        }
        else {
            binding.P3Harvest.setEnabled(false);
        }
    }

    public static void changeD4Value(Integer newValue) {
        TextView D4value = binding.P4status;
        String strValue = String.format("%d", newValue)  + " cm";
        D4value.setText(strValue);
        if(newValue >= growthThreshold) {
            binding.P4Harvest.setEnabled(true);
        }
        else {
            binding.P4Harvest.setEnabled(false);
        }
    }

    public static void changeD5Value(Integer newValue) {
        TextView D5value = binding.P5status;
        String strValue = String.format("%d", newValue)  + " cm";
        D5value.setText(strValue);
        if(newValue >= growthThreshold) {
            binding.P5Harvest.setEnabled(true);
        }
        else {
            binding.P5Harvest.setEnabled(false);
        }
    }

    public static void changeD6Value(Integer newValue) {
        TextView D5value = binding.P6status;
        String strValue = String.format("%d", newValue)  + " cm";
        D5value.setText(strValue);
        if(newValue >= growthThreshold) {
            binding.P6Harvest.setEnabled(true);
        }
        else {
            binding.P6Harvest.setEnabled(false);
        }
    }




    /**
     * Changes the EC value displayed on the screen. The value will be green if it is within
     * the acceptable value defined in the specifications [ECMINBOUND - ECMAXBOUND].
     * @param newValue the new EC value
     */
    @SuppressLint("SetTextI18n")
    public static void changeECValue(Double newValue) {
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
                harvestButton.setEnabled(true);
                break;
            case GROWING:
                label.setText("Growing");
                harvestButton.setEnabled(true);
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