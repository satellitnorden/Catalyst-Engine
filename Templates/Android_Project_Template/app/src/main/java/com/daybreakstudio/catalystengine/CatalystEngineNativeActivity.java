//Package declaration.
package com.daybreakstudio.catalystengine;

//Imports.
import android.app.NativeActivity;
import android.os.Bundle;
import android.view.Gravity;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.PopupWindow;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;

public class CatalystEngineNativeActivity extends NativeActivity
{

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        //Call parent function.
        super.onCreate(savedInstanceState);

        //Make the custom initialization.
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        MobileAds.initialize(this, new OnInitializationCompleteListener()
        {
            @Override
            public void onInitializationComplete(InitializationStatus initializationStatus)
            {

            }
        });

        //Set the activity.
        _Activity = this;

        //Create the ad view.
        _AdView = new AdView(_Activity);

        //Set the ad size.
        _AdView.setAdSize(AdSize.BANNER);

        //Set the ad unit id.
        _AdView.setAdUnitId("ca-app-pub-3940256099942544/6300978111");
    }

    @Override
    public void onDestroy()
    {
        //Destroy the ad view.
        if (_AdView != null)
        {
            _AdView.destroy();
        }

        //Call parent function.
        super.onDestroy();
    }

    public void ShowBannerAd()
    {
        if (_AdsInitialized)
        {
            return;
        }

        if (_AdView != null)
        {
            //Start the runnable.
            _Activity.runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
                    _AdsInitialized = true;

                    //Create the popup window.
                    _PopupWindow = new PopupWindow(_Activity);

                    //Set the width/height.
                    _PopupWindow.setWidth(320);
                    _PopupWindow.setHeight(50);

                    //Set the window layout mode.
                    _PopupWindow.setWindowLayoutMode(LinearLayout.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);

                    //Set clipping enabled.
                    _PopupWindow.setClippingEnabled(false);

                    //Create the layout.
                    _Layout = new LinearLayout(_Activity);

                    //Create the main layout.
                    _MainLayout = new LinearLayout(_Activity);

                    //Set the padding for the layout.
                    _Layout.setPadding(-5, -5, -5, -5);

                    //Create the margin layout params.
                    LinearLayout.MarginLayoutParams margin_layout_params = new LinearLayout.MarginLayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);

                    //Set the margins.
                    margin_layout_params.setMargins(0, 0, 0, 0);

                    //Set the orientation for the layout.
                    _Layout.setOrientation(LinearLayout.VERTICAL);

                    //Add the ad view to the layout.
                    _Layout.addView(_AdView, margin_layout_params);

                    //Set the content view for the popup window.
                    _PopupWindow.setContentView(_Layout);

                    //Set the content view for the activity.
                    _Activity.setContentView(_MainLayout, margin_layout_params);

                    //Create the ad request.
                    AdRequest ad_request = new AdRequest.Builder().build();

                    //Load the ad.
                    _Activity._AdView.loadAd(ad_request);

                    //Show the popup window.
                    _PopupWindow.showAtLocation(_MainLayout, Gravity.BOTTOM, 0, 0);

                    //Update the popup window.
                    _PopupWindow.update();
                }
            });
        }

    }

    //The activity.
    private CatalystEngineNativeActivity _Activity;

    //The ad view.
    private AdView _AdView;

    //The popup window.
    private PopupWindow _PopupWindow;

    //The layout.
    private LinearLayout _Layout;

    //The main layout.
    private LinearLayout _MainLayout;

    //Denotes whether or not ads are initialized.
    private boolean _AdsInitialized = false;

}