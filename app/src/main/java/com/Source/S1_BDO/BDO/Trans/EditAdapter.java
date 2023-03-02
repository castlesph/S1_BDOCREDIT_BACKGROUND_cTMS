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
import android.widget.CheckBox;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.R;

import java.util.Arrays;

import static com.Source.S1_BDO.BDO.Main.MultiAP_SubAP.TAG;

// sidumili: Adapter for editing fields
public class EditAdapter extends RecyclerView.Adapter<EditAdapter.MyViewHolder> {

    public static String data1[], data2[], data3[];
    public static Integer data4[];
    public static Boolean data5[];
    public static Boolean data6[];

    Context context;
    public static int row_index = -1;

    public EditAdapter(Context ct, String s1[], String s2[], String s3[], Integer s4[], Boolean s5[], Boolean s6[]){

        Log.i(TAG, "EditAdapter: run");
        context = ct;
        data1 = s1; // Field Type
        data2 = s2; // Field Description
        data3 = s3; // Field Value
        data4 = s4; // Field Length
        data5 = s5; // Flag Check Length
        data6 = s6; // isHide
    }

    @NonNull
    @Override
    public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int i) {

        Log.i(TAG, "onCreateViewHolder: run");
        LayoutInflater inflater = LayoutInflater.from(context);
        View view = inflater.inflate(R.layout.row_edit_field_layout, parent, false);

        return new MyViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull MyViewHolder holder, @SuppressLint("RecyclerView") final int position) {

        Log.i(TAG, "onBindViewHolder: run");

        holder.myCheckBox1.setVisibility(View.INVISIBLE);
        holder.editText.setVisibility(View.VISIBLE);
        holder.myText2.setText(data2[position]);

        Log.i(TAG, "BEFORE onBindViewHolder: data3="+Arrays.toString(data3));

        holder.editText.setText(data3[position]);

        Log.i(TAG, "AFTER onBindViewHolder: data3="+Arrays.toString(data3));

        //Log.i(TAG, "onBindViewHolder: data1="+Arrays.toString(data1));
        //Log.i(TAG, "onBindViewHolder: data2="+Arrays.toString(data2));
        //Log.i(TAG, "onBindViewHolder: data3="+Arrays.toString(data3));
        //Log.i(TAG, "onBindViewHolder: data4="+Arrays.toString(data4));
        //Log.i(TAG, "onBindViewHolder: data5="+Arrays.toString(data5));

        // Select item
        holder.mainLayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                row_index = position;
                notifyDataSetChanged();

                // Selected
                //Log.i("EditAdapter", "Selected: data1="+data1[position]);
                //Log.i("EditAdapter", "Selected: data2="+data2[position]);
                //Log.i("EditAdapter", "Selected: data3="+data3[position]);
                //Log.i("EditAdapter", "Selected: data4="+data4[position]);
                //Log.i("EditAdapter", "Selected: data5="+data5[position]);
                //Log.i("EditAdapter", "Selected: data6="+data6[position]);
                //Log.i("EditAdapter", "Selected: row_index="+row_index);

                String sMaxLegth = Integer.toString(data4[position]);
                String sFieldCheck = (data5[position] == true ? "1" : "0");
                String sFieldHide = (data6[position] == true ? "1" : "0");

                //Log.i("EditAdapter", "Selected: sMaxLegth="+sMaxLegth);
                //Log.i("EditAdapter", "Selected: sFieldCheck="+sFieldCheck);

                Intent intent = new Intent(context, EditListSelected.class);
                intent.putExtra("data1", data1[position]);
                intent.putExtra("data2", data2[position]);
                intent.putExtra("data3", data3[position]);
                intent.putExtra("data4", sMaxLegth);
                intent.putExtra("data5", sFieldCheck);
                intent.putExtra("data6", sFieldHide);
                intent.putExtra("dataindex", row_index);

                context.startActivity(intent);
            }
        });

        Log.i("sidumili", "onBindViewHolder: row_index="+row_index+",position="+position);
        // Set backcolor
        if (row_index == position)
        {
            Log.i("sidumili", "onBindViewHolder: Make backgroound selected");
            holder.mainLayout.setBackgroundColor(Color.parseColor("#90CAF9"));
            //holder.mainLayout.setBackgroundResource(R.drawable.shape_selected);
        }
        else
        {
            Log.i("sidumili", "onBindViewHolder: Make backgroound default");
            holder.mainLayout.setBackgroundColor(Color.parseColor("#E3F2FD"));
            //holder.mainLayout.setBackgroundResource(R.drawable.shape_gray_list);
        }

        // Hide Layout
        Log.i(TAG,   "EditAdapted->data2[position]=+"+data2[position]+",data6[position]="+data6[position]);
        if (data6[position])
            holder.mainLayout.setMaxHeight(0);
    }

    // sidumili: pass object holder and set value/label
    private void InitCheckBox(MyViewHolder holder, boolean isChecked, int position) {
        if (isChecked)
        {
            holder.myCheckBox1.setChecked(true);
            holder.myCheckBox1.setText("Enabled");
            data3[position] = "1";
        }
        else
        {
            holder.myCheckBox1.setChecked(false);
            holder.myCheckBox1.setText("Disabled");
            data3[position] = "0";
        }
    }

    @Override
    public int getItemCount() {
        return data1.length;
    }

    public class MyViewHolder extends RecyclerView.ViewHolder {

        TextView myText2;
        TextView editText;
        CheckBox myCheckBox1;
        ConstraintLayout mainLayout;

        public MyViewHolder(@NonNull View itemView) {
            super(itemView);

            Log.i(TAG, "MyViewHolder: run");

            myCheckBox1= (CheckBox) itemView.findViewById(R.id.chk_value);
            myText2 = (TextView) itemView.findViewById(R.id.tv_label);
            editText = (TextView) itemView.findViewById(R.id.tv_value);
            mainLayout = itemView.findViewById(R.id.mainLayout);
        }
    }

    private void SetData(MyViewHolder holder)
    {
        for (int i = 0; i < data3.length; i++) {
            holder.editText.setText(data3[i]);
        }
    }
}
