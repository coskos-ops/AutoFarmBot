package com.autofarmbot.server;

import com.autofarmbot.ui.plantstatus.PlantstatusFragment;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;

public class Server {

    static String apiURL = "http://coskos.pythonanywhere.com/getSensors";

    public static void startConnection() {

        Thread thread = new Thread(){
            public void run(){
                while (true) {
                    try {
                        String result = getHTML(apiURL);
                        JSONObject obj = new JSONObject(result);
                        Integer distance = (Integer) obj.get("distance");
                        Double pH = (Double) obj.get("pH");
                        Double ec = (Double) obj.get("ec");
                        PlantstatusFragment.changeDistanceValue(distance);
                        PlantstatusFragment.changePHValue(pH);
                        PlantstatusFragment.changeECValue(ec);
                        Thread.sleep(500);
                    } catch (Exception e) {
                        System.out.println(e);
                    }

                }

            }
        };
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

//        String returnString = "";
//        URL oracle = new URL(urlToRead);
//        URLConnection yc = oracle.openConnection();
//        BufferedReader in = new BufferedReader(new InputStreamReader(
//                yc.getInputStream()));
//        String inputLine;
//        while ((inputLine = in.readLine()) != null)
//            returnString += inputLine;
//        in.close();
//        return returnString;
    }
}
