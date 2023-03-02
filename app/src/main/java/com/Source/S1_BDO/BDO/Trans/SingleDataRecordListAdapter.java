package com.Source.S1_BDO.BDO.Trans;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.support.annotation.NonNull;
import android.support.constraint.ConstraintLayout;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.R;

// sidumili: Adapter for detail transaction
public class SingleDataRecordListAdapter extends RecyclerView.Adapter<SingleDataRecordListAdapter.MyViewHolder> {

    String data0[], data1[], data2[], data3[], data4[], data5[], data6[], data7[], data8[], data9[];
    Context context;
    private int lastPosition = -1;
    int row_index = -1;
    public static String final_rb_selected_string = "";
    private static final String TAG = "TransDetailAdapter";

    public static OnItemListener mOnItemListener;
    OnItemListener onItemListener;

    public SingleDataRecordListAdapter(Context ct, String s0[], String s1[], String s2[], String s3[], String s4[], String s5[], String s6[], String s7[], String s8[], OnItemListener onItemListener){
        context = ct;
        data0 = s0;
        data1 = s1;

        this.mOnItemListener = onItemListener;
    }

    @NonNull
    @Override
    public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int i) {

        LayoutInflater inflater = LayoutInflater.from(context);
        View view = inflater.inflate(R.layout.row_single_data_layout, parent, false);

        return new MyViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull final MyViewHolder holder, @SuppressLint("RecyclerView") final int position) {
        holder.box_msg.setText(data0[position]);
        holder.box_msg1.setText(data1[position]);

        // select
        holder.mainLayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                row_index = position;
                notifyDataSetChanged();

                // Selected
                Log.i(TAG, "Selected: data0="+data0[position]);
                Log.i(TAG, "Selected: data1="+data1[position]);
                Log.i(TAG, "Selected: row_index="+row_index);

                final_rb_selected_string = "";
                final_rb_selected_string = data0[position] + "^" + data1[position];

                onItemSelect(position, mOnItemListener);
            }
        });

    }

    @Override
    public int getItemCount() {
        return data1.length;
    }

    private void onItemSelect(int position, OnItemListener onItemListener)
    {
        Log.i(TAG, "SingleDataRecordListAdapter, onItemSelect: run");
        Log.i(TAG, "position="+position);

        onItemListener.onItemClick(position);
    }

    public interface OnItemListener
    {
        void onItemClick(int position);

    }
    public class MyViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

        TextView box_msg, box_msg1;
        ConstraintLayout mainLayout;

        public MyViewHolder(@NonNull View itemView) {
            super(itemView);

            box_msg = (TextView) itemView.findViewById(R.id.box_msg);
            box_msg1 = (TextView) itemView.findViewById(R.id.box_msg1);

            mainLayout = itemView.findViewById(R.id.rowConstraintLayout);

            itemView.setOnClickListener(this);
        }

        @Override
        public void onClick(View view) {
            onItemListener.onItemClick(getAdapterPosition());
        }
    }



}
