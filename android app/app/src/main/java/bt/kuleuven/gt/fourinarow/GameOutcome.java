/**
 * This file implements the functionalities of the Fast Android Networking Library.
 * Link: https://github.com/codepath/android_guides/wiki/Networking-with-the-Fast-Android-Networking-Library
 */
package bt.kuleuven.gt.fourinarow;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class GameOutcome extends AppCompatActivity {
    public String url = "http://192.168.103.113:80";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game_outcome);

        TextView textView = findViewById(R.id.textGameOutcome);
        String message = getIntent().getStringExtra("outcomeMessage");
        textView.setText(message);

        Button resetButtonOutcome = findViewById(R.id.buttonGameOutcome);
        resetButtonOutcome.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(GameOutcome.this, MainActivity.class);
                startActivity(i);
            }
        });

    }
}