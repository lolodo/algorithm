package com.example.liufangyuan.servicebestpractice;

/**
 * Created by liufangyuan on 2017/12/29.
 */

public interface DownloadListener {

    void onProgress(int progress);

    void onSuccess();

    void onFailed();

    void onPaused();

    void onCanceled();
}
