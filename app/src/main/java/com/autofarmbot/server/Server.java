package com.autofarmbot.server;

import com.autofarmbot.ui.plantstatus.PlantstatusFragment;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.concurrent.atomic.AtomicReference;

public class Server {

    static String apiURL = "http://coskos.pythonanywhere.com/getSensors";
    // Use AtomicReference for thread safety
    private static AtomicReference<Double> pH = new AtomicReference<>(0.0);
    private static AtomicReference<Double> ec = new AtomicReference<>(0.0);
    private static AtomicReference<Integer> tds = new AtomicReference<>(0);
    private static AtomicReference<Integer> temp = new AtomicReference<>(0);


    private static AtomicReference<Integer> d1 = new AtomicReference<>(0);

    private static AtomicReference<Integer> d2 = new AtomicReference<>(0);

    private static AtomicReference<Integer> d3 = new AtomicReference<>(0);

    private static AtomicReference<Integer> d4 = new AtomicReference<>(0);

    private static AtomicReference<Integer> d5 = new AtomicReference<>(0);
    private static AtomicReference<Integer> d6 = new AtomicReference<>(0);

    public static void startConnection() {
        Thread thread = new Thread(() -> {
            while (true) {
                try {
                    String result = getHTML(apiURL);
                    JSONObject obj = new JSONObject(result);
                    //Integer distance = (Integer) obj.get("distance");
                    // Update AtomicReference values
                    d1.set((Integer) obj.get("d1"));
                    d2.set((Integer) obj.get("d2"));
                    d3.set((Integer) obj.get("d3"));
                    d4.set((Integer) obj.get("d4"));
                    d5.set((Integer) obj.get("d5"));
                    d6.set((Integer) obj.get("d6"));

                    pH.set((Double) obj.get("pH"));
                    ec.set((Double) obj.get("ec"));
                    tds.set((Integer) obj.get("tds"));
                    temp.set((Integer) obj.get("temp"));

                    // Notify PlantstatusFragment about the changes
                    PlantstatusFragment.changePHValue(pH.get());
                    PlantstatusFragment.changeECValue(ec.get());
                    PlantstatusFragment.changeTDSValue(tds.get());
                    PlantstatusFragment.changeTempValue(temp.get());

                    PlantstatusFragment.changeD1Value(d1.get());
                    PlantstatusFragment.changeD2Value(d2.get());
                    PlantstatusFragment.changeD3Value(d3.get());
                    PlantstatusFragment.changeD4Value(d4.get());
                    PlantstatusFragment.changeD5Value(d5.get());
                    PlantstatusFragment.changeD6Value(d6.get());

                    Thread.sleep(1000);
                } catch (Exception e) {
                    System.out.println(e);
                }
            }
        });
        thread.start();
    }

    public static String getHTML(String urlToRead) throws Exception {
        StringBuilder result = new StringBuilder();
        URL url = new URL(urlToRead);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("GET");
        try (BufferedReader reader = new BufferedReader(
                new InputStreamReader(conn.getInputStream()))) {
            for (String line; (line = reader.readLine()) != null; ) {
                result.append(line);
            }
        }
        return result.toString();
    }

    // Public static methods to access values
    public static Double getPH() {
        return pH.get();
    }

    public static Double getEC() {
        return ec.get();
    }

    public static Integer getTDS() {
        return tds.get();
    }

    public static Integer getTemp() {
        return temp.get();
    }

    public static Integer getD1() {
        return d1.get();
    }

    public static Integer getD2() {
        return d2.get();
    }

    public static Integer getD3() {
        return d3.get();
    }

    public static Integer getD4() {
        return d4.get();
    }

    public static Integer getD5() {
        return d5.get();
    }
    public static Integer getD6() {
        return d6.get();
    }

}
