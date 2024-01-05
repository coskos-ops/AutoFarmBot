package com.autofarmbot;

import android.os.Build;
import android.webkit.WebSettings;
import android.webkit.WebView;

import androidx.fragment.app.testing.FragmentScenario;
import androidx.test.core.app.ActivityScenario;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;
import androidx.test.internal.runner.junit4.AndroidJUnit4ClassRunner;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import com.autofarmbot.ui.livefeed.LivefeedFragment;

@RunWith(AndroidJUnit4ClassRunner.class)
@LargeTest
public class LivefeedTest {

    @Before
    public void setUp() throws Exception {
        // Initialize any setup needed for the test
    }

    @Test
    public void testWebViewInitialized() {
        // Start the fragment scenario
        FragmentScenario<LivefeedFragment> scenario = FragmentScenario.launchInContainer(LivefeedFragment.class);

        // Use the fragment view to access the WebView
        scenario.onFragment(fragment -> {
            WebView webView = fragment.getView().findViewById(R.id.webview);

            // Assert that the WebView is not null
            assertNotNull(webView);

            // Assert that JavaScript is enabled
            WebSettings webSettings = webView.getSettings();
            assertTrue(webSettings.getJavaScriptEnabled());
        });
    }
}
