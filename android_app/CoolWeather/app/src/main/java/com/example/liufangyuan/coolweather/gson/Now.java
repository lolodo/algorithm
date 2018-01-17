package com.example.liufangyuan.coolweather.gson;

import com.google.gson.annotations.SerializedName;

/**
 * Created by liufangyuan on 2018/1/17.
 */

public class Now {
    @SerializedName("tmp")
    public String temperature;

    @SerializedName("cond")
    public More more;

    private class More {
        @SerializedName("txt")
        public String info;
    }
}
