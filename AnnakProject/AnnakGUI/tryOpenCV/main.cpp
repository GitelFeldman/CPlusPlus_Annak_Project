#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace cv;

// Struct to pass the world board image to the callback function
struct CallbackData {
    cv::Mat* worldBoard;
};

// Function to open and parse the input file into a vector of strings
vector<vector<string>> openInputFile() {
    string input_file = "C:/Learning/Western Digital Bootcamp/tryOpenCV/tryOpenCV/input.txt";
    ifstream ifs(input_file);
    if (!ifs.is_open())
        throw runtime_error("Failed to open the input file");

    vector<vector<string>> lines;
    string line;
    while (getline(ifs, line)) {
        istringstream stream(line);
        vector<string> words;
        string word;
        while (stream >> word) {
            words.push_back(word);
        }
        lines.push_back(words);
    }

    ifs.close();
    return lines;
}

// Function to generate a random direction (up, down, left, right)
Point randomDirection(Point pos, int cellSize) {
    int randomDirection = rand() % 4;

    switch (randomDirection) {
    case 0: // Up
        pos.y -= cellSize;
        break;
    case 1: // Down
        pos.y += cellSize;
        break;
    case 2: // Left
        pos.x -= cellSize;
        break;
    case 3: // Right
        pos.x += cellSize;
        break;
    }

    return pos;
}

// Function to get a random starting position aligned to the cell grid
int randomStartingPositions(int imageSize, int cellSize) {
    int randPos = rand() % imageSize;
    while (randPos % cellSize != 0) {
        randPos = rand() % imageSize;
    }
    return randPos;
}

// Function to move the objects randomly, checking the boundaries of the world
void moveObjectsRandomly(vector<Point>& positions, const Size& imageSize, int cellSize) {
    for (auto& pos : positions) {
        Point randPos = randomDirection(pos, cellSize);

        // Ensure the new position is within image bounds
        randPos.x = min(max(randPos.x, 0), imageSize.width - cellSize);
        randPos.y = min(max(randPos.y, 0), imageSize.height - cellSize);

        pos = randPos;
    }
}

// Function to check if a file exists
bool fileExists(const string& path) {
    ifstream file(path);
    return file.good();
}

// Mouse callback function for drawing and selecting the area to move objects
void moveObjectByChoosing(int event, int x, int y, int flags, void* param) {
    CallbackData* data = (CallbackData*)param;
    cv::Mat& worldBoard = *(data->worldBoard);

    static int ix = -1, iy = -1; // Initial selection points
    static int fx = -1, fy = -1; // Final selection points
    static bool drawing = false;
    static bool selected = false;
    static cv::Mat selectedRegion;

    if (event == EVENT_LBUTTONDOWN) {
        if (!selected) {
            drawing = true;
            ix = x;
            iy = y;
        }
        else {
            // Paste the selected area in the new position
            cv::Rect roi(x, y, selectedRegion.cols, selectedRegion.rows);
            if (roi.x >= 0 && roi.y >= 0 && roi.x + roi.width <= worldBoard.cols && roi.y + roi.height <= worldBoard.rows) {
                selectedRegion.copyTo(worldBoard(roi));
                selected = false;
                selectedRegion.release();
                ix = iy = fx = fy = -1;
            }
        }
    }
    else if (event == EVENT_MOUSEMOVE && drawing) {
        // Draw temporary rectangle during mouse move
        cv::Mat tempImage = worldBoard.clone();
        cv::rectangle(tempImage, cv::Point(ix, iy), cv::Point(x, y), cv::Scalar(0, 255, 255), 1);
        cv::imshow("World", tempImage);
    }
    else if (event == EVENT_LBUTTONUP) {
        drawing = false;
        fx = x;
        fy = y;

        // Store the selected region
        cv::Rect selectionRect(ix, iy, fx - ix, fy - iy);
        if (selectionRect.width > 0 && selectionRect.height > 0) {
            selectedRegion = worldBoard(selectionRect).clone();
            selected = true;
        }

        // Draw final rectangle on the image
        cv::rectangle(worldBoard, cv::Point(ix, iy), cv::Point(fx, fy), cv::Scalar(0, 255, 255), 1);
    }
}

int main() {
    srand(time(0));

    // Open the input file
    vector<vector<string>> lines;
    try {
        lines = openInputFile();
    }
    catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return -1;
    }

    // Define image paths and load the images
    map<int, string> imagesPath = {
        {1, "../TILES/tile_ground.png"},
        {2, "../TILES/tile_water.png"},
        {3, "../TILES/tile_forest.png"},
        {4, "../TILES/tile_field.png"},
        {5, "../TILES/tile_iron_mine.png"},
        {6, "../TILES/tile_blocks_mine.png"}
    };

    map<int, Mat> images;
    for (const auto& pair : imagesPath) {
        try {
            images[pair.first] = imread(pair.second);
            if (images[pair.first].empty()) {
                throw runtime_error("Error loading image: " + pair.second);
            }
        }
        catch (const runtime_error& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }

    int rows = lines.size();
    int cols = lines[0].size();
    int imgWidth = images[1].cols;
    int imgHeight = images[1].rows;

    // Create output image by placing tiles
    Mat outputImage(rows * imgHeight, cols * imgWidth, images[1].type());
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int lineNumber = stoi(lines[i][j]);
            Mat& img = images[lineNumber];
            Rect r((j * imgWidth), (i * imgHeight), imgWidth, imgHeight);
            img.copyTo(outputImage(r));
        }
    }

    // Load moving objects images
    vector<string> movingObjectsPaths = {
        "../VEHICLES/Audi.png",
        "../VEHICLES/truck.png",
        "../People/WALKING PERSON 1/sprite_1.png",
        "../VEHICLES/helicopter.png"
    };

    vector<Mat> movingObjects;
    for (const auto& path : movingObjectsPaths) {
        try {
            if (!fileExists(path)) {
                throw runtime_error("File does not exist: " + path);
            }
            Mat obj = imread(path, IMREAD_UNCHANGED); // Load with alpha channel
            if (obj.empty()) {
                throw runtime_error("Error loading moving object: " + path);
            }
            movingObjects.push_back(obj);
        }
        catch (const runtime_error& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }

    int cellSize = images[1].rows / 5;
    int jumpSize = outputImage.rows / (images[1].rows);

    // Resize moving objects
    resize(movingObjects[0], movingObjects[0], Size(2 * cellSize, 2 * cellSize));
    resize(movingObjects[1], movingObjects[1], Size(3 * cellSize, 3 * cellSize));
    resize(movingObjects[2], movingObjects[2], Size(cellSize, cellSize));
    resize(movingObjects[3], movingObjects[3], Size(4 * cellSize, 4 * cellSize));

    // Initialize positions of moving objects
    vector<Point> positions;
    for (size_t i = 0; i < movingObjects.size(); ++i) {
        int x = randomStartingPositions(outputImage.cols, jumpSize);
        int y = randomStartingPositions(outputImage.rows, jumpSize);
        positions.push_back(Point(x, y));
    }

    // Set up window and mouse callback
    namedWindow("World", cv::WINDOW_AUTOSIZE);
    CallbackData data = { &outputImage };
    setMouseCallback("World", moveObjectByChoosing, &data);

    // Main loop to display and move objects
    while (true) {
        Mat frame = outputImage.clone();
        for (size_t i = 0; i < movingObjects.size(); ++i) {
            if (positions[i].x >= 0 && positions[i].x + movingObjects[i].cols <= frame.cols &&
                positions[i].y >= 0 && positions[i].y + movingObjects[i].rows <= frame.rows) {
                Mat roi = frame(Rect(positions[i].x, positions[i].y, movingObjects[i].cols, movingObjects[i].rows));
                Mat mask;
                cvtColor(movingObjects[i], mask, COLOR_BGR2GRAY);
                movingObjects[i].copyTo(roi, mask);
            }
        }
        cv::imshow("World", frame);

        // Check for a key press to break the loop
        if (cv::waitKey(100) >= 0) break;

        // Move the objects randomly
        moveObjectsRandomly(positions, frame.size(), jumpSize);
    }

    return 0;
}
