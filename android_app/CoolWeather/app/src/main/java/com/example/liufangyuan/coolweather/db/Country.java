package com.example.liufangyuan.coolweather.db;

import org.litepal.crud.DataSupport;

/**
 * Created by liufangyuan on 2018/1/14.
 */

public class Country extends DataSupport {
    private int id;
    private String countryName;
    private String weatherId;
    private int cityId;

    private int getId() {
        return  id;
    }

    private void setId(int id) {
        this.id = id;
    }

    private String getCountryName() {
        return countryName;
    }

    private void setCountryName(String countryName) {
        this.countryName = countryName;
    }

    private String getWeatherId() {
        return weatherId;
    }

    private void setWeatherId(String weatherId) {
        this.weatherId = weatherId;
    }

    private int getCityId() {
        return cityId;
    }

    private  void setCityId(int cityId) {
        this.cityId = cityId;
    }
}
