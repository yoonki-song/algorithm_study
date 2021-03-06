#pragma once

// Simplified HLDPathQuery with Segment Tree
template <typename T, typename MergeOp = function<T(T, T)>>
struct SimpleHLDPathQuery {
    HLD&                hld;

    typedef CompactSegmentTree<T, MergeOp> SegTreeT;

    T                   defaultValue;
    MergeOp             mergeOp;
    vector<SegTreeT>    segTrees;

    explicit SimpleHLDPathQuery(HLD& hld, T dflt = T()) : hld(hld), mergeOp(), defaultValue(dflt) {
        // no action
    }

    SimpleHLDPathQuery(HLD& hld, MergeOp op, T dflt = T()) : hld(hld), mergeOp(op), defaultValue(dflt) {
        // no action
    }

    void build() {
        segTrees.clear();
        segTrees.reserve(hld.paths.size());

        // make segment trees on all paths
        for (const auto& path : hld.paths)
            segTrees.push_back(SegTreeT(int(path.size()), mergeOp, defaultValue));
    }

    void build(T initValue) {
        segTrees.clear();
        segTrees.reserve(hld.paths.size());

        // make segment trees on all paths
        for (const auto& path : hld.paths) {
            int m = int(path.size()) - 1;
            segTrees.push_back(SegTreeT(m + 1, mergeOp, defaultValue));
            for (int i = 0; i < m; ++i)
                segTrees.back().update(i, initValue);
        }
    }

    //--- edge-based operations

    // update a value of an edge(u-v)
    void update(int u, int v, T cost) {
        if (u >= 0 && hld.P[0][u] == v)
            swap(u, v);

        int path = hld.nodeToPath[v];
        int index = hld.indexInPath(path, v);
        segTrees[path].update(index, cost);
    }

    T query(int u, int v, int lca) const {
        return mergeOp(querySub(lca, u), querySub(lca, v));
    }

    T query(int u, int v) const {
        return query(u, v, hld.findLCA(u, v));
    }

    //--- vertex-based operations

    T rootValue = defaultValue;

    void updateVertex(int v, T cost) {
        if (v == hld.root) {
            rootValue = cost;
            return;
        }
        int path = hld.nodeToPath[v];
        int index = hld.indexInPath(path, v);
        segTrees[path].update(index, cost);
    }

    T queryVertex(int u, int v, int lca) const {
        if (lca == hld.root)
            return mergeOp(mergeOp(querySub(lca, u), querySub(lca, v)), rootValue);
        else
            return mergeOp(querySub(hld.tree.P[0][lca], u), querySub(lca, v));
    }

    T queryVertex(int u, int v) const {
        return queryVertex(u, v, hld.tree.findLCA(u, v));
    }

protected:
    // PRECONDITION: u is an ancestor of v
    // query in range (u, v]
    T querySub(int u, int v) const {
        if (u == v)
            return defaultValue;

        if (hld.nodeToPath[u] == hld.nodeToPath[v]) {
            int path = hld.nodeToPath[u];

            int first = hld.indexInPath(path, v);
            int last = hld.indexInPath(path, u) - 1;
            return segTrees[path].query(first, last);
        }

        int path = hld.nodeToPath[v];
        int topOfPath = hld.paths[path].back();

        int first = hld.indexInPath(path, v);
        return mergeOp(querySub(u, topOfPath), segTrees[path].query(first, int(hld.paths[path].size()) - 2));
    }
};

template <typename T, typename MergeOp>
inline SimpleHLDPathQuery<T, MergeOp> makeSimpleHLDPathQuery(HLD& hld, MergeOp op, T dfltValue = T()) {
    return SimpleHLDPathQuery<T, MergeOp>(hld, op, dfltValue);
}
