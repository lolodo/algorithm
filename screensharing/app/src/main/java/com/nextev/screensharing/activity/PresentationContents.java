package com.nextev.screensharing.activity;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Created by walimis on 17-10-19.
 */

public class PresentationContents implements Parcelable {

    final int[] colors;
    int displayModeId;

    public static final Creator<PresentationContents> CREATOR =
            new Creator<PresentationContents>() {
                @Override
                public PresentationContents createFromParcel(Parcel in) {
                    return new PresentationContents(in);
                }

                @Override
                public PresentationContents[] newArray(int size) {
                    return new PresentationContents[size];
                }
            };


    public PresentationContents(int[] colors) {
        this.colors = colors;
    }

    private PresentationContents(Parcel in) {
        colors = new int[]{in.readInt(), in.readInt()};
        displayModeId = in.readInt();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(colors[0]);
        dest.writeInt(colors[1]);
        dest.writeInt(displayModeId);
    }
}
