package com.autofarmbot.ui.livefeed;

import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.VideoView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.autofarmbot.R;
import com.autofarmbot.databinding.ActivityMainBinding;
import com.autofarmbot.databinding.FragmentLivefeedBinding;


public class LivefeedFragment extends Fragment {

    private FragmentLivefeedBinding binding;
    private WebView webView;

    /**
     * @param inflater           The LayoutInflater object that can be used to inflate
     *                           any views in the fragment,
     * @param container          If non-null, this is the parent view that the fragment's
     *                           UI should be attached to.  The fragment should not add the view itself,
     *                           but this can be used to generate the LayoutParams of the view.
     * @param savedInstanceState If non-null, this fragment is being re-constructed
     *                           from a previous saved state as given here.
     * @return
     */
    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        LivefeedModel livefeedModel =
                new ViewModelProvider(this).get(LivefeedModel.class);

        //automatically start the camera feed
        binding = FragmentLivefeedBinding.inflate(inflater, container, false);
        View root = binding.getRoot();

        View view = inflater.inflate(R.layout.fragment_livefeed, container, false);

        // Initialize the WebView
        webView = view.findViewById(R.id.webview);

        // Enable JavaScript (if needed)
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);

        // Load a URL (e.g., Google's homepage)
        webView.loadUrl("https://coskos.pythonanywhere.com/");

        return view;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
    }



}

