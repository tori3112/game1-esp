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
import android.widget.ListView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.androidnetworking.AndroidNetworking;
import com.androidnetworking.error.ANError;
import com.androidnetworking.interfaces.StringRequestListener;

import java.util.ArrayList;

public class GameView extends AppCompatActivity {
    public String url = "http://192.168.103.113:80";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game_view);

        ListView messageList = findViewById(R.id.gameMessageList);

        ArrayList<Object> messages = new ArrayList<>();
        MessageAdapter messageAdapter = new MessageAdapter(getApplicationContext(),messages);
        messageList.setAdapter(messageAdapter);
        String extra = "";
        AndroidNetworking.initialize(getApplicationContext());
        AndroidNetworking.get(url+"/get")
                .build()
                .getAsString(new StringRequestListener() {
                    @Override
                    public void onResponse(String response) {
                        Log.d("GameView", "Before try-catch block: response is "+response);
                        if (response.matches("[0-9]+")) {
                            Log.d("GameView", "does it pass response match");
                            messages.clear();
                            for (int i = 0; i < response.length(); i++) {
                                char move = response.charAt(i);
                                int moveValue = Character.getNumericValue(move);
                                messages.add(moveValue);
                                Log.d("GameView", "messages: "+messages);
                            }
                            messageAdapter.notifyDataSetChanged();
                        } else {
                            Log.d("GameView", "else block: response is "+response);
                        }

                    }

                    @Override
                    public void onError(ANError anError) {
                        Toast.makeText(getApplicationContext(),"inside error, something went wrong",Toast.LENGTH_LONG).show();
                    }
                });

        Button resetGame = findViewById(R.id.buttobResetGameView);
        resetGame.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(GameView.this, MainActivity.class);
                startActivity(i);
            }
        });

        Button outcomeGame = findViewById(R.id.buttonShowOutcome);
        outcomeGame.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(getApplicationContext(), GameOutcome.class);
                i.putExtra("outcomeMessage",extra);
                startActivity(i);
            }
        });



    }
}