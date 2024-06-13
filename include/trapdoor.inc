<?php
class Trapdoor {
    // general ticks
    private $ticks;

    // how long to stay open/closed
    private $openTime;
    private $closedTime;

    // open or closed
    private $state;

    // what type of trapdoor or platform
    private $type;

    // boolean indicating if the player has travelled on this space before
    private $travelled;

    public static $stateOpen = "OPEN";
    public static $stateClosed = "CLOSED";

    public static $typeTimed = "TIMED";
    public static $typeFixed = "FIXED";
    public static $typePressure = "PRESSURE";

    public function __construct($end = false) {
        // pick a random type
        $this->state = Trapdoor::$stateClosed;

        if ($end) {
            $this->type = Trapdoor::$typeFixed;
        } else {
            switch (rand(0, 2)) {
                case 0:
                    // variable open/closed
                    $this->type = Trapdoor::$typeTimed;
                    $this->openTime = rand(1000, 100000);
                    $this->closedTime = rand(5000, 200000);
         
                    if (rand(0, 1)) {
                        $this->state = Trapdoor::$stateOpen;
                    }

                    break;
                case 1:
                    // doesn't change
                    $this->type = Trapdoor::$typeFixed;

                    break;
                case 2:
                    // opens after the player stands on it long enough
                    $this->type = Trapdoor::$typePressure;
                    $this->closedTime = rand(1000, 20000);

                    break;
            }
        }

        $this->ticks = 0;
    }

    public function tick($standingOn = false) {
        $state = $this->getState();
        $type = $this->getType();

        if ($standingOn) {
            $this->travelled = true;
        }

        if ($this->type == Trapdoor::$typeTimed) {
            $this->ticks++;

            if (($state == Trapdoor::$stateClosed && $this->ticks >= $this->closedTime) || ($state == Trapdoor::$stateOpen && $this->ticks >= $this->openTime)) {
                // toggle the state
                if ($state == Trapdoor::$stateOpen) {
                    $this->state = Trapdoor::$stateClosed;
                } else {
                    $this->state = Trapdoor::$stateOpen;
                }

                $this->ticks = 0;

                return true;
            }
        } else if ($this->type == Trapdoor::$typeFixed) {
            // never changes
        } else if ($this->type == Trapdoor::$typePressure && $standingOn) {
            $this->ticks++;

            if ($state == Trapdoor::$stateClosed && $this->ticks >= $this->closedTime) {
                $this->state = Trapdoor::$stateOpen;
                
                $this->ticks = 0;

                return true;
            }
        }

        return false;
    }

    public function getState() {
        return $this->state;
    }

    public function getType() {
        return $this->type;
    }

    public function getTravelled() {
        return $this->travelled;
    }

    public function __toString() {
        if ($this->state == Trapdoor::$stateOpen) {
            $ret = " ";
        } else if ($this->type == Trapdoor::$typeFixed) {
            $ret =  json_decode('"\u2530"');
        } else {
            $ret = Ascii::horizontal();
        }

        return $ret;
    }
}