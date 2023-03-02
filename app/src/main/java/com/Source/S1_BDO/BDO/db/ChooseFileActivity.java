package com.Source.S1_BDO.BDO.db;

import android.content.res.Configuration;
import android.content.res.Resources;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import com.Source.S1_BDO.BDO.BuildConfig;
import com.Source.S1_BDO.BDO.R;

import java.util.Locale;

import timber.log.Timber;

public class ChooseFileActivity extends AppCompatActivity {

    private static final String TAG = "ChooseFileActivity";

    public ChooseFileActivity(){
        if (BuildConfig.DEBUG) {
            Timber.plant(new Timber.DebugTree());
        } else {
            //Timber.plant(new CrashReportingTree());
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choose_file);

        if (Log.isLoggable(TAG, Log.INFO)) {
            Resources resources = getApplicationContext().getResources();
            DisplayMetrics dm = resources.getDisplayMetrics();
            Configuration config = resources.getConfiguration();
            Timber.v("dm: density=" + dm.density + ", densityDpi=" + dm.densityDpi + ", curr-locale="
                + Locale.getDefault());

            // force English locale
            //config.locale = Locale.ENGLISH;
            //resources.updateConfiguration(config, dm);

            // use 正體中文
            //config.locale = Locale.TRADITIONAL_CHINESE;
            //resources.updateConfiguration(config, dm);

            // use system default
            config.locale = Locale.getDefault();
            resources.updateConfiguration(config, dm);
        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_choose_file, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
