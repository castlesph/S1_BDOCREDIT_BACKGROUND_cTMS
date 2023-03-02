package com.Source.S1_BDO.BDO.Trans;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.support.annotation.NonNull;
import android.support.constraint.ConstraintLayout;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.helper.ItemTouchHelper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.Source.S1_BDO.BDO.R;

// sidumili: Adapter for detail transaction
public class TransDetailAdapter extends RecyclerView.Adapter<TransDetailAdapter.MyViewHolder> {

    String data0[], data1[], data2[], data3[], data4[], data5[], data6[], data7[], data8[], data9[];
    Context context;
    private int lastPosition = -1;
    int row_index = -1;
    public static String final_rb_selected_string = "";
    private static final String TAG = "TransDetailAdapter";

    public TransDetailAdapter(Context ct, String s0[], String s1[], String s2[], String s3[], String s4[], String s5[], String s6[], String s7[], String s8[]){
        context = ct;
        data0 = s0;
        data1 = s1;
        data2 = s2;
        data3 = s3;
        data4 = s4;
        data5 = s5;
        data6 = s6;
        data7 = s7;
        data8 = s8;
    }

    @NonNull
    @Override
    public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int i) {

        LayoutInflater inflater = LayoutInflater.from(context);
        View view = inflater.inflate(R.layout.row_batchreview_layout, parent, false);

        return new MyViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull final MyViewHolder holder, @SuppressLint("RecyclerView") final int position) {
        holder.box_msg.setText(data0[position]);
        holder.box_msg1.setText(data1[position]);
        holder.box_msg2.setText(data2[position]);
        holder.box_msg3.setText(data3[position]);
        holder.box_msg4.setText(data4[position]);
        holder.box_msg5.setText(data5[position]);
        holder.box_msg6.setText(data6[position]);
        holder.box_msg7.setText(data7[position]);

        if (data7[position].toUpperCase().equals("VOID"))
            holder.box_msg8.setTextColor(Color.RED);
        else
            holder.box_msg8.setTextColor(Color.parseColor("#00A995"));

        holder.box_msg8.setText(data8[position]);
    }

    @Override
    public int getItemCount() {
        return data1.length;
    }

    public class MyViewHolder extends RecyclerView.ViewHolder {

        TextView box_msg, box_msg1, box_msg2, box_msg3, box_msg4, box_msg5, box_msg6, box_msg7, box_msg8;


        public MyViewHolder(@NonNull View itemView) {
            super(itemView);

            box_msg = (TextView) itemView.findViewById(R.id.box_msg);
            box_msg1 = (TextView) itemView.findViewById(R.id.box_msg1);
            box_msg2 = (TextView) itemView.findViewById(R.id.box_msg2);
            box_msg3 = (TextView) itemView.findViewById(R.id.box_msg3);
            box_msg4 = (TextView) itemView.findViewById(R.id.box_msg4);
            box_msg5 = (TextView) itemView.findViewById(R.id.box_msg5);
            box_msg6 = (TextView) itemView.findViewById(R.id.box_msg6);
            box_msg7 = (TextView) itemView.findViewById(R.id.box_msg7);
            box_msg8 = (TextView) itemView.findViewById(R.id.box_msg8);

        }
    }
}
