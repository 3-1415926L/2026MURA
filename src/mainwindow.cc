#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "NumberWalls.hpp"
#include "Morphisms.hpp"
#include "tools.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <sstream>

using namespace std;


StopWatch SW;


void applyMorphism(vector<int>& S, unordered_map<int, vector<int>>& morphism) {
    vector<int> result;
    for (auto symbol : S) {
        result.insert(result.end(), morphism[symbol].begin(), morphism[symbol].end());
    }
    S = move(result);
}


template<typename WallType>
void findMorphism(WallType& W, int morphismSize, int minUniqueIter) {

    try {
        Morphism M{W.wall, morphismSize, minUniqueIter};

        cout << "Found Morphism (canon size = "
             << M.canonicalRules.size() << ") (full size = "
             << M.countSymbols() << ")\n" << SW << endl << endl;

        ofstream ff{"temp.txt"};
        ff << "Canonical morphism\n\n";
        M.printCanonicalMorphism(ff);
        ff << "\n=========================\n\nFull morphism:\n\n";
        M.printMorphism(ff);
        ff << "\n=========================\n\nCoding:\n\n";
        M.printCoding(ff);
        cout << "Saved Morphism to temp.txt\n" << SW << endl << endl;

        //*
        bool sameZeros = M.compareZeros(W.wall);
        if (sameZeros) cout << "The morphism PASSED the verification test" << endl;
        else cout << "The morphism FAILED the verification test" << endl
                 << "Try changing parameters or including more of the sequence" << endl;
        //*/

    } catch (...) {
        cout << "Morphism not found" << endl
             << "Try changing parameters or including more of the sequence" << endl;
    }
}


template<typename WallType>
void doNumberWallStuff(WallType& W,
                       unordered_map<string, bool>& NWfeatures,
                       unordered_map<string, int>& NWargs) {

    cout << "Number Wall created" << endl;
    SW.printAndReset();
    cout << endl << endl;

    if (NWfeatures["print_text"]) {
        W.printWall();
        cout << "Wall printed" << endl;
        SW.printAndReset();
        cout << endl << endl;
    }

    if (NWfeatures["save_image"]) {
        W.savePNG("a.png", NWargs["pixel_size"]);
        cout << "Wall image saved" << endl;
        SW.printAndReset();
        cout << endl << endl;
    }

    if (NWfeatures["find_morphism"]) {
        findMorphism(W, NWargs["morphism_size"], NWargs["min_iters_for_unique"]);
        SW.printAndReset();
        cout << endl << endl;
    }

    if (NWfeatures["max_element"]) {
        cout << "Maximum element in wall = " << W.getMaxNum() << endl;
        SW.printAndReset();
        cout << endl << endl;
    }

    if (NWfeatures["max_zero_size"]) {
        cout << "Maximum zero square size = " << W.maxZeroSize() << endl;
        SW.printAndReset();
        cout << endl << endl;
    }

    if (NWfeatures["check_FC1"]) {
        bool FC1 = W.checkFC1();
        if (FC1) {
            cout << "The wall has PASSED the FC1 check" << endl;
        } else {
            cout << "The wall has FAILED the FC1 check" << endl;
        }
        SW.printAndReset();
        cout << endl << endl;
    }
}


// MainWindow & buttons

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    ui->save_image_widget->setVisible(false);
    ui->find_morphism_widget->setVisible(false);
    connect(ui->save_image, &QCheckBox::toggled,
            ui->save_image_widget, &QWidget::setVisible);
    connect(ui->find_morphism, &QCheckBox::toggled,
            ui->find_morphism_widget, &QWidget::setVisible);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_sequence_option_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}


// main Number Wall generation code

void MainWindow::on_generate_wall_button_clicked()
{
    vector<int> S;

    // Get the basic Number Wall inputs from the GUI
    int modulo = stoi(ui->modulo_in->text().toStdString());
    int seqOption = ui->sequence_option->currentIndex() + 1;
    int NWtype = stoi(ui->number_wall_type->currentText().toStdString());
    bool usingSquareWall = ui->square_wall->isChecked();


    // Get the Number Wall features from the GUI
    unordered_map<string, bool> NWfeatures;

    NWfeatures["print_text"] = ui->print_text_wall->isChecked();
    NWfeatures["save_image"] = ui->save_image->isChecked();
    NWfeatures["find_morphism"] = ui->find_morphism->isChecked();
    NWfeatures["max_element"] = ui->max_element->isChecked();
    NWfeatures["max_zero_size"] = ui->max_window_size->isChecked();
    NWfeatures["check_FC1"] = ui->check_fc1->isChecked();


    unordered_map<string, int> NWargs;

    if (NWfeatures["find_morphism"]) {
        NWargs["morphism_size"] =
            stoi(ui->morphism_size_in->text().toStdString());

        NWargs["min_iters_for_unique"] =
            stoi(ui->min_unique_iter_in->text().toStdString());
    }

    if (NWfeatures["save_image"]) {
        NWargs["pixel_size"] =
            stoi(ui->pixel_size_in->text().toStdString());
    }


    if (seqOption == 1) {

        // Option 1 - A sequence in the sequences folder

        string sequenceFile =
            ui->sequence_name->currentText().toStdString();

        int maxWidth =
            stoi(ui->max_width_in_1->text().toStdString());

        sequenceFile = "sequences/" + sequenceFile + ".txt";

        ifstream ffSeq{sequenceFile};

        string testInput;
        int element;
        int count = 0;

        while ((ffSeq >> testInput) && count < maxWidth) {
            ffSeq >> element;
            S.push_back(element);
            ++count;
        }

    } else if (seqOption == 2) {

        // Option 2 - A sequence in a specified file

        string sequenceFile =
            ui->sequence_file_path_in->text().toStdString();

        int maxWidth =
            stoi(ui->max_width_in_2->text().toStdString());

        ifstream ffSeq{sequenceFile};

        string testInput;
        int element;
        int count = 0;

        while ((ffSeq >> testInput) && count < maxWidth) {
            ffSeq >> element;
            S.push_back(element);
            ++count;
        }

    } else if (seqOption == 3) {

        // Option 3 - From a given morphism

        int startSymbol =
            stoi(ui->sequence_start_symbol_in->text().toStdString());

        int numIter =
            stoi(ui->sequence_num_iter_in->text().toStdString());

        istringstream ff{
            ui->sequence_morphism_in->toPlainText().toStdString()
        };

        unordered_map<int, vector<int>> morphism;

        int symbol, element;
        string line;

        while (ff >> symbol) {

            getline(ff, line); // this line is just the \n after symbol
            getline(ff, line);

            istringstream ll{line};

            while (ll >> element) {
                morphism[symbol].push_back(element);
            }
        }

        S = vector{startSymbol};

        for (int i = 0; i < numIter; ++i) {
            applyMorphism(S, morphism);
        }

    } else if (seqOption == 4) {

        // Option 4 - From a custom sequence

        istringstream ff{
            ui->sequence_manual_in->text().toStdString()
        };

        int element;

        while (ff >> element) {
            S.push_back(element);
        }
    }


    // zeros at the start/end of the sequence would be irrelevant in this case
    if (usingSquareWall) {
        int firstNonZero = 0;

        while (firstNonZero < S.size() && S[firstNonZero] == 0) {
            firstNonZero++;
        }

        int lastNonZero = S.size();

        while (lastNonZero > firstNonZero && S[lastNonZero - 1] == 0)
            lastNonZero--;

        S = vector<int>(S.begin() + firstNonZero,
                        S.begin() + lastNonZero);
    }


    cout << "Sequence generated" << endl;
    SW.printAndReset();
    cout << endl << endl;


    // make the Number Wall

    if (modulo == 0) {

        if (NWtype != 1 || usingSquareWall) {
            cerr << "This Number Wall type currently does not support modulus 0"
                 << endl;
            exit(1);
        }

        NumberWallNoMod W{S};
        doNumberWallStuff(W, NWfeatures, NWargs);

    }
    else if (usingSquareWall) {

        if (NWtype == 1) {

            NumberWall<FlatSquareLayout> W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);

        } else if (NWtype == 2) {

            NumberWallDet<FlatSquareLayout> W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);

        } else if (NWtype == 3) {

            NumberWallPermRyser<FlatSquareLayout> W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);

        } else if (NWtype == 4) {

            NumberWallDet3D<FlatSquareLayout> W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);

        } else {

            cerr << "Number Wall type " << NWtype
                 << " does not exist" << endl;
            exit(1);
        }

    }
    else { // if not usingSquareWall

        if (NWtype == 1) {

            NumberWall W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);

        } else if (NWtype == 2) {

            NumberWallDet W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);

        } else if (NWtype == 3) {

            NumberWallPermRyser W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);

        } else if (NWtype == 4) {

            NumberWallDet3D W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);

        } else {

            cerr << "Number Wall type " << NWtype
                 << " does not exist" << endl;
            exit(1);
        }
    }
}

