/**
 * This file implements the functionalities of the Fast Android Networking Library.
 * Link: https://github.com/codepath/android_guides/wiki/Networking-with-the-Fast-Android-Networking-Library
 */
package bt.kuleuven.gt.fourinarow;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.ArrayList;

public class MessageAdapter extends BaseAdapter {
    private Context context;
    private ArrayList positionString;

    public MessageAdapter(Context context, ArrayList positionString) {
        this.context = context;
        this.positionString = positionString;
    }

    @Override
    public int getCount() {
        return positionString.size();
    }

    @Override
    public Object getItem(int position) {
        return positionString.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            LayoutInflater layoutInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = layoutInflater.inflate(R.layout.message_item,parent,false);
        }
        Log.i("GameView","layout inflated");
        TextView textView = convertView.findViewById(R.id.gameMessageText);
        String player = ((position % 2) == 0) ? "human" : "computer";
        textView.setText((position+1)+") "+player + " moved into column "+positionString.get(position));
        return convertView;
    }

    public void updateMoves(ArrayList<Integer> moves) {
        positionString.clear();
        for (int move : moves) {
            positionString.add(String.valueOf(move));
        }
        notifyDataSetChanged();
    }
}
