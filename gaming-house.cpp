#include <iostream>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

void addOrder(string customerName);
void serveNextInQueue(string customerName);

const int MAX_TABLE = 50;
const int MAX_DEVICES = 100;
const int MAX_ORDERS = 100;
const int MAX_EDGES = 100;

// Node untuk Tree

struct Graph
{
    string devices[MAX_DEVICES];
    bool adjacencyMatrix[MAX_DEVICES][MAX_DEVICES];
    int deviceCount;

    Graph() : deviceCount(0)
    {
        for (int i = 0; i < MAX_DEVICES; ++i)
            for (int j = 0; j < MAX_DEVICES; ++j)
                adjacencyMatrix[i][j] = false;
    }
};
Graph deviceGraph;

struct TreeNode
{
    string tableNo;
    string device; // Menyimpan data PC atau Console
    string status; // Menyimpan status item (available/booked)
    double price;  // Menyimpan harga item
    TreeNode *firstchild;
    TreeNode *nextsibling;
    TreeNode *parent;
};

// Node untuk linked list dalam hash table
struct HashNode
{
    string key;
    TreeNode *treeNode;
    HashNode *next;
};

// Struktur untuk menyimpan order
struct OrderNode
{
    string orderID;
    string customerName;
    string tableNo;
    string Device;
    int pricephour;
    int hourPlay;
    int totalPrice;
    TreeNode *itemNode;
    OrderNode *next;
};

struct queueOrderData
{
    string customerName;
};
int maksOrderQueue = 10, front = 0, back = 0;
queueOrderData OrderQueue[10];

struct stackOrderData
{
    string orderID;
    string custName;
    string tableNo;
    string device;
    int pricephour;
    int hourPlay;
    int totalPrice;
};
int maksOrderStack = 100, top = 0;
stackOrderData OrderStack[100];

HashNode *itemHashTable[MAX_DEVICES] = {NULL};
OrderNode *orderHashTable[MAX_ORDERS] = {NULL};

void addDeviceToGraph(const string &deviceID)
{
    if (deviceGraph.deviceCount < MAX_DEVICES)
    {
        deviceGraph.devices[deviceGraph.deviceCount++] = deviceID;
    }
    else
    {
        cout << "\nMaksimal jumlah perangkat tercapai.\n";
    }
}

int findDeviceIndex(const string &deviceID)
{
    for (int i = 0; i < deviceGraph.deviceCount; ++i)
    {
        if (deviceGraph.devices[i] == deviceID)
        {
            return i;
        }
    }
    return -1;
}

void addEdgeToGraph(const string &device1, const string &device2)
{
    int index1 = findDeviceIndex(device1);
    int index2 = findDeviceIndex(device2);

    if (index1 != -1 && index2 != -1)
    {
        deviceGraph.adjacencyMatrix[index1][index2] = true;
        deviceGraph.adjacencyMatrix[index2][index1] = true;
        cout << "\nPerangkat berhasil dihubungkan.\n";
    }
    else
    {
        cout << "\nSalah satu atau kedua perangkat tidak ditemukan.\n";
    }
}

void removeEdgeFromGraph(const string &device1, const string &device2)
{
    int index1 = findDeviceIndex(device1);
    int index2 = findDeviceIndex(device2);

    if (index1 != -1 && index2 != -1)
    {
        deviceGraph.adjacencyMatrix[index1][index2] = false;
        deviceGraph.adjacencyMatrix[index2][index1] = false;
        cout << "\nHubungan kedua perangkat berhasil diputus.\n";
    }
    else
    {
        cout << "\nSalah satu atau kedua perangkat tidak ditemukan.\n";
    }
}

bool isConnected(string device1, string device2)
{

    int index1 = findDeviceIndex(device1);
    int index2 = findDeviceIndex(device2);

    if (index1 != -1 && index2 != -1)
    {
        return deviceGraph.adjacencyMatrix[index1][index2];
    }
    else
    {
        return false;
    }
}

void displayGraph()
{
    const int devicesPerRoom = 10;
    int roomNumber = 1;

    cout << "\nDenah Ruangan Rental:\n";
    for (int i = 0; i < deviceGraph.deviceCount; i += devicesPerRoom)
    {
        cout << "\nRuangan " << roomNumber << " : ";
        for (int j = 0; j < devicesPerRoom && (i + j) < deviceGraph.deviceCount; ++j)
        {
            if (j > 0)
            {
                string currentDeviceId = deviceGraph.devices[i + j - 1];
                string nextDeviceId = deviceGraph.devices[i + j];
                if (isConnected(currentDeviceId, nextDeviceId))
                {
                    cout << " |==| ";
                }
                else
                {
                    cout << " |  | ";
                }
            }
            cout << deviceGraph.devices[i + j];
        }
        cout << "\n";
        if ((i + devicesPerRoom) < deviceGraph.deviceCount)
        {
            cout << "|         |\n";
        }
        roomNumber++;
    }

    cout << "\n\nKoneksi Antar Perangkat:\n";
    for (int i = 0; i < deviceGraph.deviceCount; ++i)
    {
        cout << "Perangkat " << deviceGraph.devices[i] << " terhubung ke: ";
        bool first = true;
        for (int j = 0; j < deviceGraph.deviceCount; ++j)
        {
            if (deviceGraph.adjacencyMatrix[i][j])
            {
                if (!first)
                    cout << ", ";
                cout << deviceGraph.devices[j];
                first = false;
            }
        }
        cout << "\n";
    }
}

unsigned int hashFunction(string key)
{
    unsigned int hash = 0;
    for (char ch : key)
    {
        hash = (hash * 31) + ch;
    }
    return hash % MAX_TABLE;
}

void insertToItemHashTable(string key, TreeNode *treeNode)
{
    unsigned int hashIndex = hashFunction(key);
    HashNode *newNode = new HashNode{key, treeNode, itemHashTable[hashIndex]};
    itemHashTable[hashIndex] = newNode;
}

TreeNode *searchInItemHashTable(string key)
{
    unsigned int hashIndex = hashFunction(key);
    HashNode *entry = itemHashTable[hashIndex];
    while (entry != NULL)
    {
        if (entry->key == key)
        {
            return entry->treeNode;
        }
        entry = entry->next;
    }
    return NULL;
}

void insertToOrderHashTable(string key, string ID, string customerName, string tableNo, string device, int pricephour, int hour, int price, TreeNode *itemNode)
{
    unsigned int hashIndex = hashFunction(key);
    OrderNode *newNode = new OrderNode{ID, customerName, tableNo, device, pricephour, hour, price, itemNode, orderHashTable[hashIndex]};
    orderHashTable[hashIndex] = newNode;
}

OrderNode *searchInOrderHashTable(string key)
{
    unsigned int hashIndex = hashFunction(key);
    OrderNode *entry = orderHashTable[hashIndex];
    while (entry != NULL)
    {
        if (entry->tableNo == key)
        {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

OrderNode *searchIDOrderHashTable(string key)
{
    unsigned int hashIndex = hashFunction(key);
    OrderNode *entry = orderHashTable[hashIndex];
    while (entry != NULL)
    {
        if (entry->orderID == key)
        {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

TreeNode *createTreeNode(string name, string data = "", double price = 0.0, TreeNode *parent = NULL)
{
    TreeNode *newNode = new TreeNode;
    newNode->tableNo = name;
    newNode->device = data;
    newNode->status = "Available";
    newNode->price = price;
    newNode->firstchild = NULL;
    newNode->nextsibling = NULL;
    newNode->parent = parent;
    return newNode;
}

void addChild(TreeNode *parent, TreeNode *child)
{
    child->parent = parent;
    if (!parent->firstchild)
    {
        parent->firstchild = child;
    }
    else
    {
        TreeNode *temp = parent->firstchild;
        while (temp->nextsibling)
        {
            temp = temp->nextsibling;
        }
        temp->nextsibling = child;
    }
}

void collectDevices(TreeNode *node, TreeNode **devices, int &index)
{
    if (node == NULL)
        return;
    if (!node->device.empty())
        devices[index++] = node;
    collectDevices(node->firstchild, devices, index);
    collectDevices(node->nextsibling, devices, index);
}

void sortDevicesByPrice(TreeNode *root)
{
    if (root == NULL)
        return;

    int count = 0;
    TreeNode *temp = root;
    while (temp != NULL)
    {
        if (!temp->device.empty())
            count++;
        temp = temp->nextsibling;
    }

    TreeNode **devices = new TreeNode *[count];
    int index = 0;
    collectDevices(root, devices, index);

    // Bubble sort
    for (int i = 0; i < count - 1; ++i)
    {
        for (int j = 0; j < count - i - 1; ++j)
        {
            if (devices[j]->price > devices[j + 1]->price)
            {
                TreeNode *swapTemp = devices[j];
                devices[j] = devices[j + 1];
                devices[j + 1] = swapTemp;
            }
        }
    }

    // Display sorted devices
    for (int i = 0; i < count; ++i)
    {
        cout << "\n==========================\n";
        cout << "\t" << devices[i]->tableNo;
        cout << "\n==========================\n";
        cout << "\nDevice\t: " << devices[i]->device;
        cout << "\nHarga\t: Rp." << devices[i]->price;
        cout << "\n\nStatus\t: " << devices[i]->status;
    }

    delete[] devices;
}

bool isFullQueue()
{
    if (back == maksOrderQueue)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isEmptyQueue()
{
    if (back == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void enQueueOrder(string customerName)
{
    if (isFullQueue())
    {
        cout << "\nAntrian Penuh!!\n";
    }
    else
    {
        if (isEmptyQueue())
        {
            OrderQueue[0].customerName = customerName;

            front++;
            back++;
        }
        else
        {
            OrderQueue[back].customerName = customerName;
            back++;
        }
    }
}

void deQueueOrder()
{
    if (isEmptyQueue())
    {
        cout << "\nAntrian Kosong!!\n";
    }
    else
    {
        string customerName = OrderQueue[0].customerName;
        for (int i = 0; i < back; i++)
        {
            OrderQueue[i] = OrderQueue[i + 1];
        }
        back--;
        // serveNextInQueue(customerName, tableNo, hourPlay);

        serveNextInQueue(customerName);
    }
}

int countQueueOrder()
{
    if (isEmptyQueue())
    {
        return 0;
    }
    else if (isFullQueue())
    {
        return maksOrderQueue;
    }
    else
    {
        return back;
    }
}

void destroyQueueOrder()
{
    if (isEmptyQueue())
    {
        cout << "\nAntrian Kosong!!\n";
    }
    else
    {
        for (int i = 0; i < maksOrderQueue; i++)
        {
            if (back > 1)
            {
                OrderQueue[back - 1].customerName = "";
                back--;
            }
            else if (back == 1)
            {
                OrderQueue[back - 1].customerName = "";
                back = 0;
                front = 0;
            }
        }
    }
}

void viewQueueOrder()
{
    cout << "\n\nTotal in Waiting List : " << countQueueOrder() << " customer\n";
    cout << "\nOrder Queue Data : \n";
    if (isEmptyQueue())
    {
        cout << "\nAntrian Kosong!!\n";
    }
    else
    {
        for (int i = 0; i < maksOrderQueue; i++)
        {
            if (OrderQueue[i].customerName != "")
            {
                cout << "\n\nQueue No\t: " << i + 1;
                cout << "\nCustomer Name\t: " << OrderQueue[i].customerName;
            }
            else
            {
                cout << "\n\nQueue No\t: " << i + 1 << endl;
                cout << "\n        \t(Empty)";
            }
        }
    }
    cout << "\n";
}

void addToWaitingQueue()
{
    string customerName;
    cout << "\n\nEnter customer name to add to waiting queue : ";
    cin.ignore();
    getline(cin, customerName);

    enQueueOrder(customerName);
    cout << "\n\nCustomer added to waiting queue.\n";
}

void serveNextInQueue(string customerName)
{
    if (customerName == "")
    {
        cout << "\n\nNo customers in the waiting queue.\n";
    }
    else
    {
        cout << "\n\nCustomer Name : " << customerName;

        addOrder(customerName);
    }
}

bool isFullStack()
{
    if (top == maksOrderStack)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isEmptyStack()
{
    if (top == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void pushOrder(string ID, string name, string tableNo, string device, int pricephour, int playHour, int totalprice)
{
    if (isFullStack())
    {
        cout << "\nPemesanan mencapai maksimal\n";
    }
    else
    {
        OrderStack[top].orderID = ID;
        OrderStack[top].custName = name;
        OrderStack[top].tableNo = tableNo;
        OrderStack[top].device = device;
        OrderStack[top].pricephour = pricephour;
        OrderStack[top].hourPlay = playHour;
        OrderStack[top].totalPrice = totalprice;
        top++;
    }
}

void popOrder()
{
    if (isEmptyStack())
    {
        cout << "\nSejarah pemesanan tidak ditemukan.\n";
    }
    else
    {
        OrderStack[top - 1].orderID = "";
        OrderStack[top - 1].custName = "";
        OrderStack[top - 1].tableNo = "";
        OrderStack[top - 1].device = "";
        OrderStack[top - 1].pricephour = 0;
        OrderStack[top - 1].hourPlay = 0;
        OrderStack[top - 1].totalPrice = 0;
        top--;
    }
}

void displayOrderStack()
{
    if (isEmptyStack())
    {
        cout << "\nSejarah pemesanan tidak ditemukan.\n";
    }
    else
    {
        cout << "\nTotal Order : " << top;
        cout << "\nData pemesanan : \n";
        for (int i = maksOrderStack - 1; i >= 0; i--)
        {
            if (OrderStack[i].orderID != "")
            {
                cout << "\n\nData Pemesanan ke-" << i + 1 << ": \n";
                cout << "\nOrder ID\t\t: " << OrderStack[i].orderID;
                cout << "\nCustomer Name\t: " << OrderStack[i].custName;
                cout << "\nTable No\t\t: " << OrderStack[i].tableNo;
                cout << "\nDevice\t\t: " << OrderStack[i].device;
                cout << "\nPrice (/hour)\t: Rp." << OrderStack[i].pricephour;
                cout << "\nPlay hours\t: " << OrderStack[i].hourPlay << "hour";
                cout << "\nTotal Price\t: " << OrderStack[i].totalPrice;
            }
        }
        cout << "\n";
    }
}

int countOrderStack()
{
    if (isEmptyStack())
    {
        return 0;
    }
    else
    {
        return top;
    }
}

void destroyOrderStack()
{
    for (int i = 0; i < top; i++)
    {
        OrderStack[i].orderID = "";
        OrderStack[i].custName = "";
        OrderStack[i].tableNo = "";
        OrderStack[i].device = "";
        OrderStack[i].pricephour = 0;
        OrderStack[i].hourPlay = 0;
        OrderStack[i].totalPrice = 0;
    }
}

void viewSortedDevices(TreeNode *pcRoot, TreeNode *consoleRoot)
{
    cout << "\nSorting PC devices by price...\n";
    sortDevicesByPrice(pcRoot->firstchild);
    cout << "\nSorting Console devices by price...\n";
    sortDevicesByPrice(consoleRoot->firstchild);
}

void displayDevice(string tableNo, string device, string status, int price)
{
    cout << "\n\n| ========== Device Info ==========\n";
    cout << "\t\t" << tableNo;
    cout << "\n| =================================\n";
    cout << "\n| Device\t: " << device;
    cout << "\n| Harga\t: Rp." << price;
    cout << "\n\n| Status\t: " << status;
}

void deleteDevice(TreeNode *node)
{
    if (node)
    {
        deleteDevice(node->firstchild);
        deleteDevice(node->nextsibling);
        cout << "\nDevice dangan data berikut : ";
        displayDevice(node->tableNo, node->device, node->status, node->price);
        delete node;
    }
}

// Fungsi rekursif untuk traversal interaktif
void interactiveTraversal(TreeNode *node)
{
    string command;
    TreeNode *currentNode = node;

    while (true)
    {

        if (!currentNode->device.empty())
        {
            displayDevice(currentNode->tableNo, currentNode->device, currentNode->status, currentNode->price);
        }
        cout << endl;

        cout << "\n\n| ====== View Device ====== |\n";
        cout << "\n| 1. Next                   |";
        cout << "\n| 2. Previous               |";
        cout << "\n| 0. Exit                   |";
        cout << "\n>> ";
        cin >> command;

        if (command == "1")
        {
            if (currentNode->nextsibling)
            {
                interactiveTraversal(currentNode->nextsibling); // Panggil rekursif dengan saudara
            }
            else
            {
                cout << "\nTidak ada device berikutnya.\n";
            }
        }
        else if (command == "2")
        {

            if (currentNode->parent)
            {
                TreeNode *sibling = currentNode->parent->firstchild;
                if (sibling == currentNode)
                {
                    cout << "\nTidak ada device sebelumnya.\n";
                }
                else
                {
                    while (sibling && sibling->nextsibling != currentNode)
                    {
                        sibling = sibling->nextsibling;
                    }
                    if (sibling)
                    {
                        currentNode = sibling;
                    }
                    else
                    {
                        cout << "\nTidak ada device sebelumnya.\n";
                    }
                }
            }
            interactiveTraversal(currentNode);
        }
        else if (command == "0")
        {
            currentNode = node->parent;
            cout << "\nBack to main menu.\n";
            break;
        }
        else
        {
            cout << "\nPerintah tidak valid atau tidak ada node untuk dituju. Coba lagi.\n";
        }
    }
}

void addDevice(TreeNode *pcRoot, TreeNode *consoleRoot)
{
    static int pcTableCounter = 1;
    static int consoleTableCounter = 1;
    string itemType, itemNamePC, itemNameConsole;
    cout << "\nMasukkan jenis device (PC atau Console): \n";
    cout << "\n1. PC";
    cout << "\n2. Console";
    cout << "\n0. Back";
    cout << ">> ";
    cin.ignore();
    getline(cin, itemType);

    if (itemType == "1")
    {
        cout << "Pilih Device yang ingin ditambahkan\n";
        cout << "\n1. PC Standard";
        cout << "\nSpec : CPU - Intel Core i3-13100";
        cout << "\n       GPU - NVIDIA GeForce GTX 1650";
        cout << "\n       Price per hour - Rp.5000";
        cout << "\n2. PC Deluxe";
        cout << "\nSpec : CPU - Intel Core i7-13700K";
        cout << "\n       GPU - NVIDIA GeForce RTX 3090 Ti";
        cout << "\n       Price per hour - Rp.10000";
        cout << "\n3. PC Premium";
        cout << "\nSpec : CPU - Intel Core i5-13500";
        cout << "\n       GPU - NVIDIA GeForce RTX 2080 Super";
        cout << "\n       Price per hour - Rp.15000";
        cout << "\n4. PC KING";
        cout << "\nSpec : CPU - Intel Core i9-14900KS";
        cout << "\n       GPU - NVIDIA GeForce RTX 4090";
        cout << "\n       Price per hour - Rp.20000";
        cout << "\n>> ";
        cin >> itemNamePC;
        string tableName;
        if (pcTableCounter < 10)
        {
            tableName = "PC00" + to_string(pcTableCounter);
        }
        else if (pcTableCounter > 9)
        {
            tableName = "PC0" + to_string(pcTableCounter);
        }

        TreeNode *tableNode = NULL;

        if (itemNamePC == "1")
        {
            tableNode = createTreeNode(tableName, "PC Standard | Intel Core i3-13100\n      \t\t       | NVIDIA GeForce GTX 1650", 5000.0, pcRoot);
            cout << "\nPC Standard telah ditambahkan di " << tableName << ".\n";
        }
        else if (itemNamePC == "2")
        {
            tableNode = createTreeNode(tableName, "PC Deluxe | Intel Core i5-13500\n      \t\t     | NVIDIA GeForce RTX 2080 Super", 10000.0, pcRoot);
            cout << "\nPC Deluxe telah ditambahkan di " << tableName << ".\n";
        }
        else if (itemNamePC == "3")
        {
            tableNode = createTreeNode(tableName, "PC Premium | Intel Core i7-13700K\n      \t\t      | NVIDIA GeForce RTX 3090 Ti", 15000.0, pcRoot);
            cout << "\nPC Premium telah ditambahkan di " << tableName << ".\n";
        }
        else if (itemNamePC == "4")
        {
            tableNode = createTreeNode(tableName, "PC KING | Intel Core i9-14900KS\n      \t\t   | NVIDIA GeForce RTX 4090", 20000.0, pcRoot);
            cout << "\nPC KING telah ditambahkan di " << tableName << ".\n";
        }
        else
        {
            cout << "\nPilihan tidak valid\n";
            return;
        }

        addChild(pcRoot, tableNode);
        insertToItemHashTable(tableName, tableNode);
        addDeviceToGraph(tableName);

        ++pcTableCounter;
    }
    else if (itemType == "2")
    {
        cout << "Pilih Device yang ingin ditambahkan\n";
        cout << "1. PlayStation 4 \n";
        cout << "2. PlayStation 5 \n";
        cout << "3. XBOX Series S \n";
        cout << "4. XBOX Series X \n";
        cin >> itemNameConsole;

        string tableName;
        if (consoleTableCounter < 10)
        {
            tableName = "Console00" + to_string(consoleTableCounter);
        }
        else if (consoleTableCounter > 9)
        {
            tableName = "Console0" + to_string(consoleTableCounter);
        }
        TreeNode *tableNode = NULL;

        if (itemNameConsole == "1")
        {
            tableNode = createTreeNode(tableName, "PlayStation 4", 8000.0, consoleRoot);
            cout << "\nConsole tipe PlayStation 4 telah ditambahkan di " << tableName << ".\n";
        }
        else if (itemNameConsole == "2")
        {
            tableNode = createTreeNode(tableName, "PlayStation 5", 20000.0, consoleRoot);
            cout << "\nConsole tipe PlayStation 5 telah ditambahkan di " << tableName << ".\n";
        }
        else if (itemNameConsole == "3")
        {
            tableNode = createTreeNode(tableName, "XBOX Series S", 10000.0, consoleRoot);
            cout << "\nConsole tipe XBOX Series S telah ditambahkan di " << tableName << ".\n";
        }
        else if (itemNameConsole == "4")
        {
            tableNode = createTreeNode(tableName, "XBOX Series X", 15000.0, consoleRoot);
            cout << "\nConsole tipe XBOX Series X telah ditambahkan di " << tableName << ".\n";
        }
        else
        {
            cout << "\nPilihan tidak valid\n";
            return;
        }

        addChild(consoleRoot, tableNode);
        insertToItemHashTable(tableName, tableNode);
        addDeviceToGraph(tableName);

        ++consoleTableCounter;
    }
    else
    {
        cout << "\nJenis item tidak valid. Hanya bisa PC(1) atau Console(2).\n";
    }
}

int generateID()
{
    int randomNumber;

    srand(time(NULL));
    for (int i = 0; i < 6; i++)
    {
        randomNumber = (rand() * rand() + rand() * rand()) % 100000;
        randomNumber += 100000;
    }
    int ID = randomNumber;
    return ID;
}

void displayOrder(string orderID, string customerName, string tableNo, string device, int devprice, int hourPlay, int priceTotal)
{
    cout << "\n\n| ==================================";
    cout << "\n| Order ID : " << orderID;
    cout << "\n| __________________________________";
    cout << "\n| Customer Name\t: " << customerName;
    cout << "\n| Table No\t\t: " << tableNo;
    cout << "\n| Device\t\t: " << device;
    cout << "\n| Price (/hour)\t: " << devprice;
    cout << "\n| Play hours\t: " << hourPlay;
    cout << "\n| Price Total\t: " << priceTotal;
    cout << "\n| ==================================";
}

void addOrder(string customerName = "")
{
    string tableNo;
    int hourPlay;
    cout << "\n\nMasukkan nomor meja (misal: PC001): ";
    cin.ignore();
    getline(cin, tableNo);

    TreeNode *itemNode = searchInItemHashTable(tableNo);
    if (itemNode)
    {
        displayDevice(itemNode->tableNo, itemNode->device, itemNode->status, itemNode->price);
        if (itemNode->status == "Available")
        {
            cout << "\n\n|========== Order ==========|";
            if (customerName == "")
            {
                cout << "\nMasukkan nama pelanggan: ";
                getline(cin, customerName);
            }
            cout << "\nMasukkan Jam bermain: ";
            cin >> hourPlay;
            int priceTotal = hourPlay * itemNode->price;
            string device = itemNode->device;
            int pricephour = itemNode->price;
            itemNode->status = "Played";
            string orderID = to_string(generateID());
            insertToOrderHashTable(tableNo, orderID, customerName, tableNo, device, pricephour, hourPlay, priceTotal, itemNode);
            displayOrder(orderID, customerName, tableNo, device, pricephour, hourPlay, priceTotal);
            pushOrder(orderID, customerName, tableNo, device, pricephour, hourPlay, priceTotal);
            cout << "Order berhasil ditambahkan.\n";
        }
        else
        {
            cout << "\n\nMeja tersebut sedang digunakan.\n";
        }
    }
    else
    {
        cout << "\n\nMeja tidak ditemukan.\n";
    }
}

void endOrder()
{
    string tableNo;
    cout << "\nMasukkan nomor meja (misal: PC001): ";
    cin.ignore();
    getline(cin, tableNo);

    TreeNode *itemNode = searchInItemHashTable(tableNo);
    if (itemNode)
    {

        if (itemNode->status == "Played")
        {

            itemNode->status = "Available";

            cout << "\n\nOrder berhasil diakhiri.\n";
        }
        else
        {
            cout << "\n\nMeja tersebut tidak sedang digunakan.\n";
        }
    }
    else
    {
        cout << "\n\nMeja tidak ditemukan.\n";
    }
}

void MainMenu(TreeNode *pcRoot, TreeNode *consoleRoot)
{
    int choice;
    do
    {

        cout << "\n\n| ============ Main Menu ============ |\n";
        cout << "\n| 1. Tambah Device ke Meja            |";
        cout << "\n| 2. View PC Device                   |";
        cout << "\n| 3. View Console Device              |";
        cout << "\n| 4. Sort Devices by Price            |";
        cout << "\n| 5. Search Table by Number           |";
        cout << "\n| 6. Add Order                        |";
        cout << "\n| 7. End order                        |";
        cout << "\n| 8. Add to Waiting List              |";
        cout << "\n| 9. View Order Queue                 |";
        cout << "\n| 10. View Order History              |";
        cout << "\n| 11. Tambah koneksi antara perangkat |";
        cout << "\n| 12. Hapus koneksi antara perangkat  |";
        cout << "\n| 13. Tampilkan denah perangkat       |";
        cout << "\n| 0. Keluar                           |";
        cout << "\n| =================================== |\n";
        cout << "\n>> ";

        string tableNo;
        cin >> choice;

        if (choice == 1)
        {
            addDevice(pcRoot, consoleRoot);
        }
        else if (choice == 2)
        {

            if (pcRoot->firstchild)
            {
                interactiveTraversal(pcRoot->firstchild);
            }
            else
            {
                cout << "\nBelum ada meja PC yang ditambahkan.\n";
            }
        }
        else if (choice == 3)
        {
            if (consoleRoot->firstchild)
            {
                interactiveTraversal(consoleRoot->firstchild);
            }
            else
            {
                cout << "\nBelum ada meja Console yang ditambahkan.\n";
            }
        }
        else if (choice == 4)
        {
            sortDevicesByPrice(pcRoot->firstchild);
            sortDevicesByPrice(consoleRoot->firstchild);
        }
        else if (choice == 5)
        {
            cout << "\nMasukkan nomor meja (misal : PC001): ";
            cin.ignore();
            getline(cin, tableNo);

            TreeNode *result = searchInItemHashTable(tableNo);
            if (result)
            {
                cout << "\nMeja ditemukan : " << result->tableNo;
                cout << "\nDevice\t: " << result->device;
                cout << "\nStatus\t: " << result->status;
                cout << "\nHarga\t: Rp." << result->price;
            }
            else
            {
                cout << "\nMeja tidak ditemukan.\n";
            }
        }
        else if (choice == 6)
        {
            addOrder();
        }
        else if (choice == 7)
        {
            endOrder();
        }
        else if (choice == 8)
        {
            addToWaitingQueue();
        }
        else if (choice == 9)
        {
            do
            {
                viewQueueOrder();
                cout << "\n\n| ====== Waiting List ====== |";
                cout << "\n| 1. Tambah Order dari Antrian |";
                cout << "\n| 9. Kembali                   |";
                cout << "\n>> ";
                cin >> choice;
                switch (choice)
                {
                case 1:
                    deQueueOrder();
                    break;
                case 9:
                    cout << "\nBack to Previous Menu\n";
                default:
                    cout << "\nInvalid choice.\n";
                    break;
                }
            } while (choice != 9);
        }
        else if (choice == 10)
        {
            do
            {
                displayOrderStack();
                cout << "\n\n| ====== Order History ====== |";
                cout << "\n| 1. Remove Latest Order      |";
                cout << "\n| 2. Remove All Order Hisory  |";
                cout << "\n| 9. Kembali                  |";
                cout << "\n>> ";
                cin >> choice;
                switch (choice)
                {
                case 1:
                    popOrder();
                    cout << "\nLatest Order Data has been removed.\n";
                    break;
                case 2:
                    destroyOrderStack();
                    cout << "\nAll Order Data has been removed.\n";
                    break;
                case 9:
                    cout << "\nBack to Previous Menu\n";
                default:
                    cout << "\nInvalid choice.\n";
                    break;
                }
            } while (choice != 9);
        }
        else if (choice == 11)
        {
            string device1, device2;
            cout << "\nMasukkan ID perangkat pertama : ";
            cin >> device1;
            cout << "\nMasukkan ID perangkat kedua : ";
            cin >> device2;
            addEdgeToGraph(device1, device2);
        }
        else if (choice == 12)
        {
            string device1, device2;
            cout << "\nMasukkan ID perangkat pertama : ";
            cin >> device1;
            cout << "\nMasukkan ID perangkat kedua : ";
            cin >> device2;
            removeEdgeFromGraph(device1, device2);
        }
        else if (choice == 13)
        {
            displayGraph();
        }
        else if (choice == 0)
        {
            cout << "\n\nExiting Program . . .";
            break;
        }
        else
        {
            cout << "\nsInvalid Choice.\n";
        }
    } while (choice != 0);
}

int main()
{
    // Membuat root dari sistem rental
    TreeNode *root = createTreeNode("Rental");

    // Membuat sub-root untuk PC dan Console
    TreeNode *pcRoot = createTreeNode("PCs");
    TreeNode *consoleRoot = createTreeNode("Consoles");

    // Menambahkan sub-root ke dalam tree
    addChild(root, pcRoot);
    addChild(root, consoleRoot);

    // Memanggil menu utama
    MainMenu(pcRoot, consoleRoot);

    // Membersihkan memory
    delete root;
    delete pcRoot;
    delete consoleRoot;

    return 0;
}
