package com.autofarmbot;

import static android.app.PendingIntent.getActivity;
import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.*;
import static androidx.test.espresso.matcher.ViewMatchers.withId;

import androidx.test.filters.LargeTest;
import androidx.test.rule.ActivityTestRule;
import androidx.test.runner.AndroidJUnit4;

import com.autofarmbot.plant.Plant;
import com.autofarmbot.plant.PlantStatus;
import com.autofarmbot.plant.Position;
import com.autofarmbot.ui.plantstatus.PlantstatusFragment;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class PlantStatusTest {



    @Rule
    public ActivityTestRule<MainActivity> mActivityRule =
            new ActivityTestRule<>(MainActivity.class);


    @Test
    public void testPlantView() throws Throwable {
        onView(withId(R.id.navigation_plantstatus)).perform(click());

        for(int i = 1; i < 10; i++) {
            int finalI = i;
            runOnUI(() -> {PlantstatusFragment.changePHValue(finalI);});
            Thread.sleep(300);
        }

        for(float f = 1.0F; f < 2; f += 0.1) {
            float finalF = f;
            runOnUI(() -> {PlantstatusFragment.changeECValue(finalF);});
            Thread.sleep(300);
        }

        Plant P1 = new Plant("P1", Position.plantPositions.get("P1"), PlantStatus.GROWING);
        runOnUI(() -> {PlantstatusFragment.changePlantStatus(P1);});
        Thread.sleep(1000);

        P1.setStatus(PlantStatus.RIPE);
        runOnUI(() -> {PlantstatusFragment.changePlantStatus(P1);});
        Thread.sleep(1000);

        P1.setStatus(PlantStatus.EMPTY);
        runOnUI(() -> {PlantstatusFragment.changePlantStatus(P1);});
        Thread.sleep(1000);


    }

    private void runOnUI(Runnable r) {
        try {
            mActivityRule.runOnUiThread(r);
        }
        catch (Throwable e) {

        }
    }



}