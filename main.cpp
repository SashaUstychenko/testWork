#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <bitset>
/*
You are given a locked container represented as a two-dimensional grid of boolean values (true = locked, false = unlocked).
Your task is to write an algorithm that fully unlocks the box, i.e.,
transforms the entire matrix into all false.

Implement the function:
bool openBox(uint32_t x, uint32_t y);
This function should:
    - Use the SecureBox public API (toggle, isLocked, getState).
    - Strategically toggle cells to reach a state where all elements are false.
    - Return true if the box remains locked, false if successfully unlocked.
You are not allowed to path or modify the SecureBox class.

Evaluation Criteria:
    - Functional correctness
    - Computational efficiency
    - Code quality, structure, and comments
    - Algorithmic insight and clarity
*/

class SecureBox
{
private:
    std::vector<std::vector<bool>> box;

public:

    //================================================================================
    // Constructor: SecureBox
    // Description: Initializes the secure box with a given size and 
    //              shuffles its state using a pseudo-random number generator 
    //              seeded with current time.
    //================================================================================
    SecureBox(uint32_t y, uint32_t x) : ySize(y), xSize(x)
    {
        rng.seed(time(0));
        box.resize(y);
        for (auto& it : box)
            it.resize(x);
        shuffle();
    }

    //================================================================================
    // Method: toggle
    // Description: Toggles the state at position (x, y) and also all cells in the
    //              same row above and the same column to the left of it.
    //================================================================================
    void toggle(uint32_t y, uint32_t x)
    {
        box[y][x] = !box[y][x];
        for (uint32_t i = 0; i < xSize; i++)
            box[y][i] = !box[y][i];
        for (uint32_t i = 0; i < ySize; i++)
            box[i][x] = !box[i][x];
    }

    //================================================================================
    // Method: isLocked
    // Description: Returns true if any cell 
    //              in the box is true (locked); false otherwise.
    //================================================================================
    bool isLocked()
    {
        for (uint32_t x = 0; x < xSize; x++)
            for (uint32_t y = 0; y < ySize; y++)
                if (box[y][x])
                    return true;

        return false;
    }

    //================================================================================
    // Method: getState
    // Description: Returns a copy of the current state of the box.
    //================================================================================
    std::vector<std::vector<bool>> getState()
    {
        return box;
    }

private:
    std::mt19937_64 rng;
    uint32_t ySize, xSize;

    //================================================================================
    // Method: shuffle
    // Description: Randomly toggles cells in the box to 
    // create an initial locked state.
    //================================================================================
    void shuffle()
    {
        for (uint32_t t = rng() % 1000; t > 0; t--)
            toggle(rng() % ySize, rng() % xSize);
    }
};

//================================================================================
// Function: openBox
// Description: Your task is to implement this function to unlock the SecureBox.
//              Use only the public methods of SecureBox (toggle, getState, isLocked).
//              You must determine the correct sequence of toggle operations to make
//              all values in the box 'false'. The function should return false if
//              the box is successfully unlocked, or true if any cell remains locked.
//================================================================================

bool openBox(uint32_t ySize, uint32_t xSize)
{

    const int MAX_N = 10000; // 100 x 100 максимум
    SecureBox box(ySize, xSize);
    auto state = box.getState();

    int N = ySize * xSize;
    std::vector<std::bitset<MAX_N + 1>> matrix(N); //розширенна матриця для метода гауса
  
    // Побудова матриці кофіцієнтів
    for (uint32_t row = 0; row < ySize; ++row)
    {
        for (uint32_t col = 0; col < xSize; ++col)
        {
            int idx = row * xSize + col;

            // toggle(row, col) впливає на:
            // - усі в row
            // - усі в col

            matrix[idx][idx] = 1; // self
            //Перебираємо весь рядок row. Тиснучи на клітинку, ми також змінюємо всі
            //клітинки в цьому рядку — тобто дописуємо 1 до відповідних змінних.

            //!bit працює як XOR над 1: тобто, якщо вже стоїть 1, скидає в 0, і навпаки.
            for (uint32_t i = 0; i < xSize; ++i)
                matrix[idx][row * xSize + i] = !matrix[idx][row * xSize + i]; // row
            for (uint32_t i = 0; i < ySize; ++i)
                matrix[idx][i * xSize + col] = !matrix[idx][i * xSize + col]; // col

            // Права частина — чи заблокована ця клітинка
            matrix[idx][MAX_N] = state[row][col];
        }
    }

    //  виконуємо метод Гауса по модулю 2 
    int row = 0;
    for (int col = 0; col < N; ++col)
    {
      //Шукаємо перший рядок з 1 в поточному стовпц
        int sel = -1;
        for (int i = row; i < N; ++i)
        {
            if (matrix[i][col])
            {
                sel = i;
                break;
            }
        }

        //Якщо не знайшли жодного то переходимо до наступного стовпця
        if (sel == -1) continue;

        std::swap(matrix[row], matrix[sel]);
        for (int i = 0; i < N; ++i)
        {
            if (i != row && matrix[i][col])//Обнуляємо всі інші 1 в цьому стовпці за допомогою XOR.
                matrix[i] ^= matrix[row];
        }
        row++;
    }

    std::vector<bool> press(N, false);//вектор press, в якому true означає, що потрібно натиснути на цю клітинку.
    for (int i = 0; i < N; ++i)
        press[i] = matrix[i][MAX_N];

    // Застосовуємо toggle() для всіх, де натискати
    for (uint32_t r = 0; r < ySize; ++r)
    {
        for (uint32_t c = 0; c < xSize; ++c)
        {
            int i = r * xSize + c;
            if (press[i])
                box.toggle(r, c);
        }
    }

    return box.isLocked(); 
}



int main(int argc, char* argv[])
{
    uint32_t x = std::atol(argv[1]);
    uint32_t y = std::atol(argv[2]);
    bool state = openBox(y, x);

    if (state)
        std::cout << "BOX: LOCKED!" << std::endl;
    else
        std::cout << "BOX: OPENED!" << std::endl;

    return state;
}


