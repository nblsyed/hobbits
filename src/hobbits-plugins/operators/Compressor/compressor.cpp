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
#include <QMap>
#include <QMessageBox>
using namespace std;

Compressor::Compressor() :
    ui(new Ui::Compressor())
{

}

QVector<QString> decoded;

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
    ui->cb_type->addItem("Huffman 8-bit");
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
    QString ch;
    int freq;
    Node *left, *right;

};


//allocate a new tree node
Node* getNode(QString ch, int freq, Node* left, Node* right)
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
            QMap<QString, string> &huffmanCode)
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
Node* makeTree(QVector<QString> letters, QVector<int> nums) {

    priority_queue<Node*, vector<Node*>, comp> pq;

    //make leaf node for each character and add it to priority queue
    for (int i = 0; i < letters.size(); i++) {
        pq.push(getNode(letters.at(i), nums.at(i), nullptr, nullptr));
    }

    // do till there is more than one node in the queue
    while (pq.size() != 1)
    {
        //remove 2 nodes of lowest frequency
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top();	pq.pop();

        //make internal node
        int sum = left->freq + right->freq;
        pq.push(getNode("", sum, left, right));
    }

    //root stores pointer to root of Huffman Tree
    Node* root = pq.top();
    return root;
}



void load(QByteArray inputBytes, QSharedPointer<const BitArray> input){

    int encSize = (inputBytes.mid(3, (inputBytes.indexOf("|")-3))).toInt();

    QVector<int> freq;
    QVector<QVector<bool>> uniq;
    QVector<bool> garb;
    int startF = inputBytes.indexOf("|");
    int endF = inputBytes.indexOf("^");


    //get the frequencies
    for(int i = startF+1; i < endF; i++){
        if(inputBytes.at(i)=='{'){
            QString num;
            int end = inputBytes.indexOf('}', i);
            for(int j = i+1; j < end; j++){

                num.append(inputBytes.at(j));
            }
            i = end;
            freq.append(num.toInt());
        }else{
            freq.append((int)inputBytes.at(i));
        }
    }

    //get the 8-bit codes
    int startC = (endF+1)*8;
    int endC = inputBytes.indexOf("||")*8;
    for(int i = startC; i < endC; i+=8){
        QVector<bool> b;
        for(int j = i; j < i+8; j++){
            b.append(input->at(j));
        }
        uniq.append(b);
    }


    //get the encoded string
    string str;
    for(int i = (inputBytes.indexOf("||")+2)*8; i < ((inputBytes.indexOf("||")+2)*8)+encSize; i++){
        garb.append(input->at(i));
        if(input->at(i)){
            str+="1";
        }else{
            str+="0";
        }
    }

    QVector<QString> uniq2;
    QString qs = "";
    for(QVector<bool> qb : uniq){
        for(bool b : qb){
            if(b){
                qs.append("1");
            }else{
                qs.append("0");
            }
        }
        uniq2.append(qs);
        qs.clear();
    }

    //makes the tree
    Node* root = makeTree(uniq2, freq);

    //stores codes
    QMap<QString, string> huffmanCode;
    encode(root, "", huffmanCode);



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



    QByteArray data = inputBits->getPreviewBytes();

    //can use to experiment with 16 bit or 32 bit based encryption
    //larger bit means smaller huffman code but with larger metadata
    //bit must be divisible by 8
    int bit = 8;

    //determine if file is encoded orn't
    if(data.at(0) == '~' && data.at(2) == '~'){
        load(data, inputBits);

        QString dec;
        for(QString q : decoded){
            dec.append(q);
        }
        QSharedPointer<BitArray> output = QSharedPointer<BitArray>(new BitArray(dec.size()));
        for(int i = 0; i < dec.size(); i++){
            if(dec.at(i) == "0"){
                output->set(i, 0);
            }else{
                output->set(i, 1);
            }
        }
        bitContainer->setBits(output);
        outputContainers.append(bitContainer);
    }else{


    QString bits = "";

    QVector<QString> hex;
    QVector<QString> uniq;
    QVector<int> freq;

    //convert to string of bits
    //
    for(int i = 0; i < inputBits->sizeInBits(); i++){
        if(inputBits->at(i)){
            bits.append("1");
        }else{
            bits.append("0");
        }
    }

    //extract all bytes
    for(int i = bit; i < bits.size() - (bits.size()%bit)+1; i+=bit){
        if(i%bit == 0){
            hex.append(bits.mid(i-bit, bit));
        }
    }

    //unlikely to happen
    if(bits.size()%bit!=0){
        hex.append(bits.mid(bits.size()-(bits.size()%bit), bits.size()));
    }

    //remove repetitions
    for(int i = 0; i < hex.size(); i++){
        if(!uniq.contains(hex.at(i))){
            uniq.append(hex.at(i));
            i = 0;
        }
    }

    sort(uniq.begin(), uniq.end());

    //get frequencies
    for(QString q : uniq){
        freq.append(hex.count(q));
    }


    Node* root = makeTree(uniq, freq);

    //stores codes
    QMap<QString, string> huffmanCode;
    encode(root, "", huffmanCode);


/*

    QMapIterator<QString, string> i(huffmanCode);
    while (i.hasNext()) {
        i.next();
        //cout << i.key().toStdString() << ": " << i.value() << endl;
    }
*/


    //encoded binary in string
    string str = "";
    for (QString ch: hex) {
        str += huffmanCode[ch];
    }

    //get long list of 8bit characters
    QVector<bool> chars;
    for(QString q : uniq){
        for(QChar qq : q){
            if(qq == '0'){
                chars.append(0);
            }else{
                chars.append(1);
            }
        }
    }


    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(chars.size()));
    QSharedPointer<BitArray> keys = QSharedPointer<BitArray>(new BitArray(str.length()));

    //8 bit occurances
    for(int i = 0; i < chars.size(); i++){
        outputBits->set(i, chars.at(i));
    }

    //encoded garbage
    for(int i = 0; i < str.length(); i++){
        if(str[i] == '0'){
            keys->set(i, 0);
        }else{
            keys->set(i, 1);
        }

    }



    //get metadata ready
    QByteArray out;
    out.append("~");
    out.append(QString::number(bit/8));
    // 8-1 16-2
    out.append("~");

    //size of encoded string
    out.append(QString::number(str.length()));
    out.append("|");

    //frequency
    for(int f : freq){
        if(f>=123){
            out.append("{");
            out.append(QString::number(f));
            out.append("}");
        }else{
            out.append(f);
        }
    }
    out.append("^");

    //8 bit occurances
    out.append(outputBits->getPreviewBytes());

    out.append("||");

    //encoded garbage
    out.append(keys->getPreviewBytes());

    bitContainer->setBits(out);
    outputContainers.append(bitContainer);

    }
    return OperatorResult::result({outputContainers}, recallablePluginState);
}

void Compressor::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
    // optionally use the current container to prepare the UI or something
}
