pumpPin = 'D14';
redPin  = 'D26';
bluePin = 'D36';

function newDir = turnLeft(dir)
% turnLeft  Rotate direction vector 90 degrees left.
    newDir = [-dir(2) dir(1)];
end

function newDir = turnRight(dir)
% turnRight  Rotate direction vector 90 degrees right.
    newDir = [dir(2) -dir(1)];
end

function pos = generateFood(boardSize, snake)
% generateFood  Return a random grid cell not taken up by the snake.
    while true
        pos = [randi(boardSize) randi(boardSize)];
        if ~ismember(pos, snake, 'rows')
            return;
        end
    end
end

boardSize = 20;          % 20x20 grid

while true
    % Wait for start / restart
    lcdClear(m5core);
    lcdCursor(m5core, 10, 40);
    lcdPrint(m5core, 'Press any button');
    lcdCursor(m5core, 10, 60);
    lcdPrint(m5core, 'to start the game.');

    % wait until red or blue is pressed (active-low)
    waiting = true;
    while waiting
        redVal  = readDigitalPin(esp32, redPin);
        blueVal = readDigitalPin(esp32, bluePin);
        if redVal == 0 || blueVal == 0
            waiting = false;
        end
    end

    lcdClear(m5core);
    lcdCursor(m5core, 10, 40);
    lcdPrint(m5core, 'Game is started');

    % Initialize game state
    snake      = [boardSize/2 boardSize/2];   % start in middle
    direction  = [0 1];    
    snakeLength = 3;
    gameOver   = false;

    food = generateFood(boardSize, snake);

    figure(67); clf;
    axis([1 boardSize 1 boardSize]);
    grid on; hold on;
    set(gca, 'XTick', 1:boardSize, 'YTick', 1:boardSize);
    title('Snake Game of Punishment and Dispair');

    % Game loop
    while ~gameOver
        % read buttons to see which direction to go
        redVal  = readDigitalPin(esp32, redPin);
        blueVal = readDigitalPin(esp32, bluePin);

        if redVal == 0
            direction = turnLeft(direction);
        elseif blueVal == 0
            direction = turnRight(direction);
        end

        % move snake
        newHead = snake(1,:) + direction;
        snake   = [newHead; snake];      % add new head

        if size(snake,1) > snakeLength
            snake(end,:) = [];           % remove tail
        end

        % collision with wall or self?
        hitWall = newHead(1) < 1 || newHead(1) > boardSize || ...
                  newHead(2) < 1 || newHead(2) > boardSize;
        hitSelf = ismember(newHead, snake(2:end,:), 'rows');

        if hitWall || hitSelf
            gameOver = true;
            break;
        end

        % check food
        if isequal(newHead, food)
            snakeLength = snakeLength + 1;
            food = generateFood(boardSize, snake);
        end

        % create a new frame of the game
        cla;
        plot(snake(:,1), snake(:,2), 'g.', 'MarkerSize', 25);
        hold on;
        plot(food(1), food(2), 'rx', 'MarkerSize', 20, 'LineWidth', 3);
        axis([1 boardSize 1 boardSize]);
        drawnow;
        pause(0.20);   % game speed
    end

    % Game over!!!
    lcdClear(m5core);
    lcdCursor(m5core, 5, 40);
    lcdPrint(m5core, 'You lost! haha!');
    lcdCursor(m5core, 5, 60);
    lcdPrint(m5core, 'Press any button');
    lcdCursor(m5core, 5, 80);
    lcdPrint(m5core, 'to restart!');

    fprintf("You lost! haha! press any button to restart!\n");

    % Negative feedback: activate pump for 3 seconds to squirt on the
    % player
    PumpOn(esp32, pumpPin);
    pause(3);
    PumpOff(esp32, pumpPin);

    % Wait for button press to restart
    waitingRestart = true;
    while waitingRestart
        redVal  = readDigitalPin(esp32, redPin);
        blueVal = readDigitalPin(esp32, bluePin);
        if redVal == 0 || blueVal == 0
            waitingRestart = false;
        end
    end
end