/*
    Copyright (c) 2020, RootHarold
    All rights reserved.

    Use of this source code is governed by a LGPL-3.0 license that can be found
    in the LICENSE file.
*/

#ifndef LYCORIS_GENOME_H
#define LYCORIS_GENOME_H

#include <cstdint>
#include <string>

/*
 * The connection between nodes in the neural network is composed
 * of the class Gen and the class Ome. A connection is divided
 * into two objects to facilitate the use of std::map in forward
 * computing.
 */

namespace LycorisNet {

    class Lycoris;

    /*
     * The class Gen contains the number of the forward node and the
     * number of the backward node.
     */

    class Gen {
    public:
        Gen();

        Gen(uint32_t in, uint32_t out);

        // Overloading operator.
        inline bool operator<(const Gen &gen) const {
            if (in < gen.in) {
                return true;
            } else if (in == gen.in) {
                return out < gen.out;
            } else {
                return false;
            }
        }

        friend class LycorisUtils;

        friend class Lycoris;

        friend class Individual;

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

        friend Lycoris *loadViaString(const std::string &model, uint32_t capacity);

    private:
        // The number of the forward node.
        uint32_t in;
        // The number of the backward node.
        uint32_t out;
    };

    /*
     * The class Ome contains connection weights, connection numbers,
     * gradients, and the backups of gradients (used in parallel computing).
     */

    class Ome {
    public:
        Ome();

        Ome(float weight, uint32_t innovationNum);

        friend class LycorisUtils;

        friend class Lycoris;

        friend class Individual;

        friend Lycoris *loadModel(const std::string &path, uint32_t capacity);

        friend Lycoris *loadViaString(const std::string &model, uint32_t capacity);

    private:
        // Connection weight.
        float weight;
        // Gradient.
        float delta;
        // The backup of gradient.
        float delta_backup;
        // The number of the connection.
        uint32_t innovationNum;
    };

}

#endif //LYCORIS_GENOME_H
