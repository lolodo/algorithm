package com.example.liufangyuan.coolweather.gson;

import com.google.gson.annotations.SerializedName;

/**
 * Created by liufangyuan on 2018/1/17.
 */

public class Basic {
    @SerializedName("city")
    public String cityName;

    @SerializedName("id")
    public String weatherId;

    public Update update;

    public class Update {
        @SerializedName("Ioc")
        public String updateTime;
    }
}
