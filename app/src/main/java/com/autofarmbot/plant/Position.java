package com.autofarmbot.plant;

import java.util.HashMap;

/**
 * Class encapsulating a position of a Plant in space
 */
public class Position {

    /**
     * HashMap containing the position of each plant
     */
    public static final HashMap<String, Position> plantPositions = new HashMap<>();

    static {
        // ToDo: change these numbers to something meaningful
        plantPositions.put("P1", new Position(1, 2, 3));
        plantPositions.put("P2", new Position(1, 2, 3));
        plantPositions.put("P3", new Position(1, 2, 3));
        plantPositions.put("P4", new Position(1, 2, 3));
    }

    /**
     * X coordinate in space
     */
    private final int x;
    /**
     * Y coordinate in space
     */
    private final int y;

    /**
     * Z coordinate in space
     */
    private final int z;

    /**
     * The constructor of Position is private and thus an external instance cannot be created
     */
    private Position(int x, int y, int z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
}
