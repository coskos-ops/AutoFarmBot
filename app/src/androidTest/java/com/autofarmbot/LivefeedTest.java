package com.autofarmbot;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.matcher.ViewMatchers.withId;

import androidx.test.espresso.Espresso;
import androidx.test.espresso.action.ViewActions;
import androidx.test.espresso.matcher.ViewMatchers;
import androidx.test.rule.ActivityTestRule;

import com.autofarmbot.plant.Plant;
import com.autofarmbot.plant.PlantStatus;
import com.autofarmbot.plant.Position;
import com.autofarmbot.ui.plantstatus.PlantstatusFragment;

import org.junit.Rule;
import org.junit.Test;

public class LivefeedTest {
    @Rule
    public ActivityTestRule<MainActivity> mActivityRule =
            new ActivityTestRule<>(MainActivity.class);


    @Test
    public void testPlantView() throws Throwable {
        onView(withId(R.id.navigation_plantstatus)).perform(click());

        for(int i = 1; i < 10; i++) {
            int finalI = i;
            runOnUI(() -> {
                PlantstatusFragment.changePHValue(finalI);});
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

        onView(withId(R.id.navigation_home)).perform(click());
        Thread.sleep(500);

        onView(withId(R.id.navigation_livefeed)).perform(click());
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