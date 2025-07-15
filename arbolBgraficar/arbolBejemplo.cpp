#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath> // Para usar ceil

class BTree;

class Node {
private:
    std::vector<int> keys;
    std::vector<Node*> children;
    bool leaf;
    int m; // Orden del árbol (Grado Máximo)

    friend class BTree;

public:
    Node(int order, bool is_leaf);
    void traverse(); // Función para recorrer y mostrar (opcional)
    void insertNonFull(int k);
    void splitChild(int i, Node* y);
    void generateNodeGraphviz(std::ofstream& out);
};

class BTree {
private:
    Node* root;
    int m; // Orden del árbol (Grado Máximo)

    void destroy(Node* node) {
        if (!node) return;
        if (!node->leaf) {
            for (Node* child : node->children) {
                destroy(child);
            }
        }
        delete node;
    }

public:
    BTree(int order) : m(order), root(nullptr) {
        if (m < 3) {
            throw std::invalid_argument("El orden de un Árbol-B debe ser al menos 3.");
        }
    }

    ~BTree() {
        destroy(root);
    }

    void insert(int k);
    void generateGraphviz(const std::string& filename);
};

// --- Implementación de Node ---

Node::Node(int order, bool is_leaf) : m(order), leaf(is_leaf) {}

void Node::insertNonFull(int k) {
    int i = keys.size() - 1;

    if (leaf) {
        // Encontrar la posición para insertar la nueva llave
        auto posToInsert = std::upper_bound(keys.begin(), keys.end(), k);
        keys.insert(posToInsert, k);
    } else {
        // Encontrar el hijo que va a recibir la llave
        // Ajustamos el bucle para encontrar el índice correcto del hijo
        while (i >= 0 && keys[i] > k) {
            i--;
        }
        int child_index = i + 1; // Este es el índice del puntero al hijo

        // Si el hijo está lleno, lo dividimos
        if (children[child_index]->keys.size() == m - 1) {
            splitChild(child_index, children[child_index]);
            // Después de dividir, la llave mediana subió.
            // Decidimos a cuál de los dos nuevos hijos bajar.
            if (k > keys[child_index]) {
                child_index++;
            }
        }
        children[child_index]->insertNonFull(k);
    }
}

void Node::splitChild(int i, Node* y) {
    // 'y' es el nodo lleno que se va a dividir.
    // 'this' es el padre de 'y'.
    // 'i' es el índice de 'y' en el vector de hijos del padre.
    
    Node* z = new Node(y->m, y->leaf);

    // La nueva llave que subirá al padre es la mediana del nodo 'y'
    // Para un orden m, el número máximo de llaves es m-1.
    // La llave mediana será la que está en la posición (m-1)/2
    int median_index = (m - 1) / 2;
    int medianKey = y->keys[median_index];

    // Las llaves a la derecha de la mediana van al nuevo nodo 'z'
    // Copiar desde median_index + 1 hasta el final
    for (int j = median_index + 1; j < y->keys.size(); ++j) {
        z->keys.push_back(y->keys[j]);
    }
    
    // Las llaves a la izquierda de la mediana (incluyendo la mediana si no se mueve)
    // Se quedan en 'y'. Redimensionar 'y' para que solo tenga las llaves correctas.
    y->keys.resize(median_index); // Esto elimina la llave mediana y las de la derecha

    // Si 'y' no era una hoja, reasignamos sus hijos
    if (!y->leaf) {
        // Los hijos a la derecha de la mediana (median_index + 1) van al nuevo nodo 'z'
        // El número de hijos es m.
        // La mitad de los hijos (ceil(m/2.0) si es par, o m/2+1 si es impar)
        // se quedan con y, el resto va a z.
        // Los hijos que van a 'z' empiezan desde el índice 'median_index + 1' en 'y'
        for (int j = median_index + 1; j < y->children.size(); ++j) {
            z->children.push_back(y->children[j]);
        }
        y->children.resize(median_index + 1); // Redimensionar 'y' para que solo tenga los hijos correctos
    }

    // Insertamos el nuevo hijo 'z' en el nodo padre en la posición i + 1
    children.insert(children.begin() + i + 1, z);

    // Insertamos la llave mediana en el nodo padre en la posición i
    keys.insert(keys.begin() + i, medianKey);
}

// --- Implementación de BTree ---

void BTree::insert(int k) {
    if (!root) {
        root = new Node(m, true);
        root->keys.push_back(k);
    } else {
        // Si la raíz está llena, el árbol crece en altura
        if (root->keys.size() == m - 1) {
            Node* new_root = new Node(m, false); // La nueva raíz no será una hoja
            new_root->children.push_back(root);
            new_root->splitChild(0, root);
            
            // Decidir en cuál de los dos hijos de la nueva raíz insertar
            int i = 0;
            if (new_root->keys[0] < k) { // Si k es mayor que la llave que subió a la nueva raíz
                i++; // Insertar en el segundo hijo
            }
            new_root->children[i]->insertNonFull(k);
            root = new_root;
        } else {
            root->insertNonFull(k);
        }
    }
}

// --- Lógica de Graficación Corregida y Robusta ---

void Node::generateNodeGraphviz(std::ofstream& out) {
    long nodeId = reinterpret_cast<long>(this); // Identificador único para el nodo
    out << "  node" << nodeId << " [shape=record, label=\"";

    // Crear una celda para cada puntero y cada llave
    // Para un nodo con 'k' llaves, necesitamos 'k+1' punteros/celdas si no es una hoja.
    // Si es una hoja, solo las llaves.
    if (!leaf) { // Si no es una hoja, necesita los punteros de los hijos
        for (size_t i = 0; i < keys.size(); ++i) {
            out << "<p" << i << "> | " << keys[i] << " | ";
        }
        out << "<p" << keys.size() << ">\"];" << std::endl;
    } else { // Si es una hoja, solo las llaves
        for (size_t i = 0; i < keys.size(); ++i) {
            out << keys[i];
            if (i < keys.size() - 1) {
                out << " | ";
            }
        }
        out << "\"];" << std::endl;
    }

    // Si no es una hoja, dibujar las aristas hacia los hijos
    if (!leaf) {
        // Recorrer todos los hijos y conectarles desde el puerto correspondiente
        for (size_t i = 0; i < children.size(); ++i) {
            long childId = reinterpret_cast<long>(children[i]);
            out << "  \"node" << nodeId << "\":p" << i << " -> \"node" << childId << "\";" << std::endl;
            
            // Llamada recursiva para que cada hijo se dibuje a sí mismo
            children[i]->generateNodeGraphviz(out);
        }
    }
}

// La función BTree::generateGraphviz se mantiene igual, no es necesario cambiarla
void BTree::generateGraphviz(const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error abriendo el archivo: " << filename << std::endl;
        return;
    }
    out << "digraph BTree {" << std::endl;
    out << "  node [shape=record, style=filled, fillcolor=skyblue];" << std::endl;
    if (root) {
        root->generateNodeGraphviz(out);
    }
    out << "}" << std::endl;
    out.close();
}


// --- Main ---

int main() {
    // Creamos un Árbol-B de ORDEN 3 (máximo 2 llaves por nodo)
    BTree t(3); 
    
    int values[] = {8, 9, 10, 11, 15, 16, 17, 18, 20, 23,1,2,34,56,12,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    for (int val : values) {
        t.insert(val);
    }
    
    t.generateGraphviz("b_tree_corrected.dot");
    std::cout << "Archivo Graphviz 'b_tree_corrected.dot' generado." << std::endl;
    std::cout << "Para generar la imagen, usa el comando:" << std::endl;
    std::cout << "dot -Tpng b_tree_corrected.dot -o b_tree_corrected.png" << std::endl;
    
    return 0;
}