package com.autofarmbot.ui.livefeed;

import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
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
    private VideoView videoView;

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

        // Initialize the VideoView
        videoView = view.findViewById(R.id.video_view);

        // Set the path to the video file (assuming it's named "sample.mp4" in the res/raw folder)
        String videoPath = "android.resource://" + getActivity().getPackageName() + "/" + R.raw.img_7474;
        videoView.setVideoURI(Uri.parse(videoPath));

        // Start playing the video
        videoView.start();

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

    /**
     * @param menu     The options menu in which you place your items.
     * @param inflater
     */
    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.live_feed_menu, menu); //initiate menu for camera switch
        super.onCreateOptionsMenu(menu, inflater);
    }

    /**
     * @param item The menu item that was selected.
     * @return
     *
     * Switch between different camera views based on user preference (on button click)
     * Need to replace videos with live camera views later when backend is ready
     *
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int itemId = item.getItemId();
        if (itemId == R.id.camera_1) {
            String videoPath = "android.resource://" + getActivity().getPackageName() + "/" + R.raw.img_7474;
            videoView.setVideoURI(Uri.parse(videoPath));
            videoView.start();
            Toast.makeText(getContext(), "Camera View 1 Selected", Toast.LENGTH_LONG).show();
            return true;
        } else if (itemId == R.id.camera_2) {
            String videoPath = "android.resource://" + getActivity().getPackageName() + "/" + R.raw.img_7475;
            videoView.setVideoURI(Uri.parse(videoPath));
            videoView.start();
            Toast.makeText(getContext(), "Camera View 2 Selected", Toast.LENGTH_LONG).show();
            return true;
        } else if (itemId == R.id.camera_3) {
            String videoPath = "android.resource://" + getActivity().getPackageName() + "/" + R.raw.img_7476;
            videoView.setVideoURI(Uri.parse(videoPath));
            videoView.start();
            Toast.makeText(getContext(), "Camera View 3 Selected", Toast.LENGTH_LONG).show();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

}

