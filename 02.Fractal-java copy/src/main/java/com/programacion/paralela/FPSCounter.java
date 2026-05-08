package com.programacion.paralela;

public class FPSCounter {

    private int fps;
    private int frames;
    private long lasTime;

    public FPSCounter() {
        lasTime = System.currentTimeMillis();
        fps = 0;
        frames = 0;
    }

    public int update() {
        frames++;
        long now = System.currentTimeMillis();

        if (now - lasTime > 1000) {
            fps = frames;
            frames = 0;
            lasTime = now;
        }
        return fps;
    }
}
