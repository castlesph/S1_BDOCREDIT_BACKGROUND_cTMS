package com.Source.S1_BDO.BDO.DMenu;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.R;

import java.util.List;

/**
 * Created by lijuan on 2016/9/12.
 */
public class GridViewAdapter extends BaseAdapter {
    private List<Model> mDatas;
    private LayoutInflater inflater;
    /**
     * 页数下标,从0开始(当前是第几页)
     */
    private int curIndex;
    /**
     * 每一页显示的个数
     */
    private int pageSize;

    private int viewMode;

    public GridViewAdapter(Context context, List<Model> mDatas, int curIndex, int pageSize, int viewMode) {
        inflater = LayoutInflater.from(context);
        this.mDatas = mDatas;
        this.curIndex = curIndex;
        this.pageSize = pageSize;
        this.viewMode = viewMode;
    }

    /**
     * 先判断数据集的大小是否足够显示满本页？mDatas.size() > (curIndex+1)*pageSize,
     * 如果够，则直接返回每一页显示的最大条目个数pageSize,
     * 如果不够，则有几项返回几,(mDatas.size() - curIndex * pageSize);(也就是最后一页的时候就显示剩余item)
     */
    @Override
    public int getCount() {
        return mDatas.size() > (curIndex + 1) * pageSize ? pageSize : (mDatas.size() - curIndex * pageSize);

    }

    @Override
    public Object getItem(int position) {
        return mDatas.get(position + curIndex * pageSize);
    }

    @Override
    public long getItemId(int position) {
        return position + curIndex * pageSize;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder viewHolder = null;
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.item_gridview, parent, false);
            viewHolder = new ViewHolder();
            viewHolder.tv = (TextView) convertView.findViewById(R.id.textView);
            viewHolder.iv = (ImageView) convertView.findViewById(R.id.imageView);
            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) convertView.getTag();
        }
        /**
         * 在给View绑定显示的数据时，计算正确的position = position + curIndex * pageSize，
         */
        int pos = position + curIndex * pageSize;

        // (1 - for text mode, 2 - for image mode) -- sidumili
        switch (viewMode)
        {
            case 1: // text mode
                viewHolder.iv.setVisibility(View.GONE);
                viewHolder.tv.setTextSize(16.0f);
                break;
            case 2: // image mode
                break;
        }

        viewHolder.tv.setText(mDatas.get(pos).name);
        viewHolder.iv.setImageResource(mDatas.get(pos).iconRes);

        // set image layout for width/height for wallet -- sidumili
        for (int i = 0; i < mDatas.size(); i++)
        {
            String sTemp = mDatas.get(i).name;
            if (sTemp.contains("Wechat") ||
                sTemp.contains("QRPh") ||
                sTemp.contains("Alipay") ||
                sTemp.contains("GCashCreditGives") ||
                sTemp.contains("GrabPay") ||
                sTemp.contains("BDO Pay"))
            {
                android.view.ViewGroup.LayoutParams layoutParams = viewHolder.iv.getLayoutParams();
                layoutParams.width = ViewGroup.LayoutParams.MATCH_PARENT;
                layoutParams.height = ViewGroup.LayoutParams.MATCH_PARENT;
                viewHolder.iv.setLayoutParams(layoutParams);
            }
        }

        return convertView;
    }


    class ViewHolder {
        public TextView tv;
        public ImageView iv;
    }
}