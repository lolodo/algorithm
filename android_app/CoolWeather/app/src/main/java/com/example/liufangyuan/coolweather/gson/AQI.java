package com.example.liufangyuan.coolweather.gson;

/**
 * Created by liufangyuan on 2018/1/17.
 */

public class AQI {
    public AQICity city;

    private class AQICity {
        public String aqi;
        public String pm25;
    }
}
