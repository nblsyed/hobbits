#include "compressor.h"
#include "ui_compressor.h"
#include <QObject>
#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include<QDebug>
#include <vector>
#include <cstdlib>

using namespace std;

Compressor::Compressor() :
    ui(new Ui::Compressor())
{

}

QVector<char> decoded;
OperatorInterface* Compressor::createDefaultOperator()
{
    return new Compressor();
}

//Return name of operator
QString Compressor::getName()
{
    return "Compressor";
}

void Compressor::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

void Compressor::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
    ui->cb_type->addItem("Huffman Latin1");
    //connect(ui->rd_enc, SIGNAL(clicked()), this, SLOT(showHelp()));
}

bool Compressor::canRecallPluginState(const QJsonObject &pluginState)
{
    //if pluginState does not have required fields, return false
    if(pluginState.isEmpty()==true){
        return false;
    }

    return true;
}

bool Compressor::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    // Set the UI fields based on the plugin state
    ui->custom->setText(pluginState.value("message").toString());
    ui->cb_type->addItem("Huffman Latin1");
    //ui->rd_enc->setChecked(false);
    //ui->rd_dec->setChecked(true);
    return true;
}

QJsonObject Compressor::getStateFromUi()
{
    QJsonObject pluginState;

    //Pull data from the input fields and input them into pluginState
    pluginState.insert("message", ui->custom->text());
    return pluginState;
}

int Compressor::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int Compressor::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

// /////////////////////////////////////////////////////////////////////////////////////////

struct Node
{
    char ch;
    int freq;
    Node *left, *right;

};


//allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right)
{
    Node* node = new Node();

    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}

//comparison object to be used to order the heap
struct comp
{
    bool operator()(Node* l, Node* r)
    {
        // highest priority item has lowest frequency
        return l->freq > r->freq;
    }
};

//traverse the Huffman Tree and store Huffman Codes in a map.
void encode(Node* root, string str,
            unordered_map<char, string> &huffmanCode)
{
    if (root == nullptr)
        return;

    // found a leaf node
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

// traverse the Huffman Tree and decode the encoded string
void decode(Node* root, int &index, string str)
{
    if (root == nullptr) {
        return;
    }
    // found a leaf node
    if (!root->left && !root->right)
    {
        decoded.append(root->ch);
        return;
    }

    index++;

    if (str[index] =='0')
        decode(root->left, index, str);
    else
        decode(root->right, index, str);
}


// /////////////////////////////////////////////////////////////////////////////////////////


//makes binary tree from raw characters and their frequencies
Node* makeTree(QString letters, QVector<int> nums) {

    priority_queue<Node*, vector<Node*>, comp> pq;

    //make leaf node for each character and add it to priority queue
    for (int i = 0; i < letters.size(); i++) {
        pq.push(getNode(letters.at(i).toLatin1(), nums.at(i), nullptr, nullptr));
    }

    // do till there is more than one node in the queue
    while (pq.size() != 1)
    {
        //remove 2 nodes of lowest frequency
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top();	pq.pop();

        //make internal node
        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }

    //root stores pointer to root of Huffman Tree
    Node* root = pq.top();
    return root;
}

//decodes metadata from encrypted file
void loadPrefix(QString huffCode, QByteArray bytes, QSharedPointer<const BitArray> input){

    huffCode.remove(0,2);
    int id = huffCode.indexOf("]?[");
    int sizeId = huffCode.indexOf("[?]");
    int encSize = (huffCode.mid(id+3, sizeId-(id+3))).toInt();
    QString code = huffCode.left(id);
    int id2 = bytes.indexOf("[?]")+3;

    QString letters;
    QVector<int> nums;
    QString number;
    for(int i = 2; i < huffCode.indexOf("]?["); i++){

        if(code.at(i-2)=="." && code.at(i-1) == ","){

            letters.append(code.at(i));

        }else if(code.at(i-2)=="." && code.at(i-1)=="/"){
            int c = i;
            while(code.at(c)!="/"){
                number.append(code.at(c));
                c+=1;
            }
            nums.append(number.toInt());
            number.clear();
            i=c;
        }
    }

    Node* root = makeTree(letters, nums);


    // traverse the Huffman Tree and store Huffman Codes
    // in a map. Also prints them
    unordered_map<char, string> huffmanCode;
    encode(root, "", huffmanCode);


    //encoded string
    string str = "";
    for(int i = id2*8; i < encSize+(id2*8); i++){
        if(input->at(i)){
            str+="1";
        }else{
            str+="0";
        }
    }



    // decode the encoded string
    int index = -1;
    while (index < (int)str.size() - 2) {
        decode(root, index, str);
    }

}



QSharedPointer<const OperatorResult> Compressor::operateOnContainers(
        QList<QSharedPointer<const BitContainer> > inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    //QSharedPointer<OperatorResult> result(new OperatorResult());
    //QSharedPointer<const OperatorResult> nullResult;
    QList<QSharedPointer<BitContainer>> outputContainers;
    QSharedPointer<const BitArray> inputBits = inputContainers.takeFirst()->bits();
    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    QString message = recallablePluginState.value("message").toString();
    decoded.clear();

    //
    if(message.isEmpty()){
        message.append(inputBits->getPreviewBytes());
    }



    if(message.at(0) == "&" && message.at(1) =="&"){
        loadPrefix(message, inputBits->getPreviewBytes(), inputBits);
        message.clear();
        for(int i = 0; i < decoded.size(); i++){
            message.append(decoded.at(i));
        }
        bitContainer->setBits(message.toLatin1());
        outputContainers.append(bitContainer);

    }else{

        QString text = message;
        QString pre = "";


        QString letters;

        QVector<int> nums;

        //append unique characters to QString letters
        for(int i = 0; i < text.size(); i++){
            if(!letters.contains(text.at(i))){
                letters.append(text.at(i));
                i = 0;
            }
        }

        sort(letters.begin(), letters.end());

        //store frequencies of each letter in QVector nums
        for(int i = 0; i < letters.size(); i++){
            nums.append( text.count(letters.at(i))  );
        }


        pre.append("&&");
        for(int i = 0; i < letters.size(); i++){
            pre.append(".,");
            pre.append(letters.at(i));
            pre.append("./");
            pre.append(QString::number(nums.at(i)));
            pre.append("/");
        }

        //make a tree
        Node* root = makeTree(letters, nums);


        //stores codes
        unordered_map<char, string> huffmanCode;
        encode(root, "", huffmanCode);


        //encoded binary in string
        string str = "";
        for (char ch: text.toStdString()) {
            str += huffmanCode[ch];
        }

        //set output bits from encoded binary
        QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(str.length()));
        for(int i = 0; i < str.length(); i++){
            if(str[i] == '0'){
                outputBits->set(i, 0);


            }else{
                outputBits->set(i, 1);

            }
        }


        QByteArray encoded = pre.toLatin1();
        encoded.append("]?[");
        encoded.append(QString::number(outputBits->sizeInBits()));
        encoded.append("[?]");
        encoded.append(outputBits->getPreviewBytes());




        bitContainer->setBits(encoded);
        outputContainers.append(bitContainer);

    }

    return OperatorResult::result({outputContainers}, recallablePluginState);
}

void Compressor::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
    // optionally use the current container to prepare the UI or something
}
