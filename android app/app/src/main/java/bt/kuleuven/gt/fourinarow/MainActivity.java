/**
 * This file implements the functionalities of the Fast Android Networking Library.
 * Link: https://github.com/codepath/android_guides/wiki/Networking-with-the-Fast-Android-Networking-Library
 */

package bt.kuleuven.gt.fourinarow;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.androidnetworking.AndroidNetworking;
import com.androidnetworking.error.ANError;
import com.androidnetworking.interfaces.StringRequestListener;

import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity {
    public String url = "http://192.168.103.113:80";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        AndroidNetworking.initialize(getApplicationContext());

        Button startGame = findViewById(R.id.buttonStartGame);
        startGame.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String data = "game is ready";
                JSONObject object = new JSONObject();
                try {
                    object.put("message",data);
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                AndroidNetworking.post(url+"/post")
                        .addJSONObjectBody(object)
                        .build()
                        .getAsString(new StringRequestListener() {
                            @Override
                            public void onResponse(String response) {
                                Log.d("MINE",response.toString());
                                Toast.makeText(getApplicationContext(), response.toString(), Toast.LENGTH_LONG).show();
                                Intent i = new Intent(MainActivity.this, GameView.class);
                                startActivity(i);
                            }

                            @Override
                            public void onError(ANError anError) {
                                Toast.makeText(getApplicationContext(), "something went wrong, onError", Toast.LENGTH_LONG);
                                Log.e("MINE","something wrong: "+anError.getErrorDetail() );
                            }
                        });
            }
        });

        Button resetGame = findViewById(R.id.buttonReset);
        resetGame.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               Intent i = new Intent(MainActivity.this, MainActivity.class);
               startActivity(i);
            }
        });

    }
}