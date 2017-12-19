package com.example.alex.networktest;

/**
 * Created by alex on 17-12-19.
 */

public interface HttpCallbackListener {
    void onFinish(String response);
    void onError(Exception e);
}
