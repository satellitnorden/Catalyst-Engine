package com.chrysalisinteractive.catalystenginetemplate;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class Main extends AppCompatActivity
{

    static
    {
        //Load the necessary native libraries.
        //System.loadLibrary("catalyst-engine-lib");
        //System.loadLibrary("clairvoyant-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

}
