<?php
class Game {
    // players term settings to restore after exit
    private $term;

    // holds the trapdoor objects
    private $path;

    // where on the path is the player? path[pos]
    private $pos;

    // how wide is the current path
    private $width;

    // does the player have to get to the left, or the right
    private $dest;

    // an integer player score
    private $score;

    public function __construct() {
        $this->term = `stty -g`;
		system("stty -icanon -echo");

        $this->init();

        stream_set_blocking(STDIN, false); // Do not wait

        $this->gameLoop();
    }

    public function init() {
        // add some trapdoors to the path
        $this->score = 0;
        $this->dest = "RIGHT";
        $this->width = 40;
        $this->pos = 0;

        $this->preparePath();
    }
    
    private function preparePath() {
        $this->path = array();

        for ($x = 0; $x < $this->width; $x++) {
            $this->path[] = new Trapdoor($x == 0 || $x == ($this->width - 1));
        }
    }

    private function advance() {
        $this->score = $this->score + 100;

        if ($this->dest == "LEFT") {
            $this->dest = "RIGHT";
        } else {
            $this->dest = "LEFT";
        }

        $this->preparePath();
    }

    public function gameLoop() {
        echo $this;

        while (1) {
            $redraw = $this->tick();

            $c = Config::waitForInput();

            switch ($c) {
                case 'q':
                    $this->gameOver();
                    break;
                case Keypress::leftArrow():
                case Keypress::doubleLeftArrow():
                    if ($this->pos > 0) {
                        $this->pos--;
                        $redraw = true;

                        if (!$this->path[$this->pos]->getTravelled()) {
                            $this->score++;
                        }
                    }

                    if ($this->pos === 0 && $this->dest == "LEFT") {
                        $this->advance();
                    }

                    break;
                case Keypress::rightArrow():
                case Keypress::doubleRightArrow():
                    if ($this->pos < $this->width - 1) {
                        $this->pos++;
                        $redraw = true;

                        if (!$this->path[$this->pos]->getTravelled()) {
                            $this->score++;
                        }
                    }

                    if ($this->pos === $this->width - 1 && $this->dest == "RIGHT") {
                        $this->advance();
                    }

                    break;
                default:
                    // unhandled keypress
                    if ($c != '') {
                        echo "unhandled keypress: ";
                        print_r(Config::strToChr($c));
                    }
            }

            if ($redraw) {
                echo $this;
            }
        }

        $this->gameOver();
    }

    private function tick() {
        $this->checkPos();

        $redraws = array();

        for ($x = 0; $x < count($this->path); $x++) {
            $redraws[] = $this->path[$x]->tick($this->pos == $x);
        }

        if (in_array(true, $redraws)) {
            return true;
        }

        return false;
    }

    private function gameOver() {
        echo "GAME OVER!\n";

        system("stty " . $this->term);

        exit;
    }

    private function checkPos() {
        $td = $this->path[$this->pos];

        if ($td->getState() == Trapdoor::$stateOpen) {
            $this->gameOver();
        }
    }

    public function __toString() {
        //system("clear");

        $ret = "\n";

        // score bar
        $ret .= "score: " . $this->score . " direction: ";
        if ($this->dest == "LEFT") {
            $ret .= Ascii::arrowLeft();
        } else {
            $ret .= Ascii::arrowRight();
        }

        $ret .= "\n";

        // main game area
        for ($x = 0; $x < $this->width; $x++) {
            if ($x == $this->pos) {
                $ret .= json_decode('"\u23b5"');
            } else {
                $ret .= " ";
            }
        }

        $ret .= "\n";

        for ($x = 0; $x < $this->width; $x++) {
            $ret .= $this->path[$x];    
        }

        $ret .= "\n";

        return $ret;
    }
}