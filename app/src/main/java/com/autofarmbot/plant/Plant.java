package com.autofarmbot.plant;

/**
 * Class encapsulating a plant
 */
public class Plant {
    /**
     * Plant name
     */
    private final String name;
    /**
     * Plant position
     */
    private final Position pos;
    /**
     * Plant status
     */
    private PlantStatus status;

    /**
     * Constructor
     * @param name Plant name
     * @param pos Plant position
     * @param status Plant status
     */
    public Plant(String name, Position pos, PlantStatus status) {
        this.name = name;
        this.pos = pos;
        this.status = status;
    }

    /**
     * Getter for pos
     * @return pos
     */
    public Position getPos() {
        return pos;
    }

    /**
     * Getter for name
     * @return name
     */
    public String getName() {
        return name;
    }

    /**
     * Setter for status
     * @param s The new status
     */
    public void setStatus(PlantStatus s) {
        status = s;
    }

    /**
     * Getter for status
     * @return status
     */
    public PlantStatus getStatus() {
        return status;
    }


}
