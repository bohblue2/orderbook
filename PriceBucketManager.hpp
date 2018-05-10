//
// Created by Uncle Chu on 9/5/18.
//

#ifndef ORDERBOOK_PRICEBUCKETMANAGER_HPP
#define ORDERBOOK_PRICEBUCKETMANAGER_HPP

#include <map>
#include <utility>
#include <memory>
#include <iostream>
#include "PriceBucket.h"

class default_bucket_set
{
    // wrapper class which forward calls to std::map, just to conform with
    // our api contract with PriceBucketManager
    using bucket_set_t = std::map<uint64_t, std::shared_ptr<PriceBucket>>;

public:

    std::shared_ptr<PriceBucket> find( uint64_t price )
    {
        auto item = m_map.find( price );
        if ( item != m_map.end() )
            return item->second;
        else
            return std::shared_ptr<PriceBucket>();
    }

    std::shared_ptr<PriceBucket> successor( uint64_t price )
    {
        auto item = m_map.upper_bound( price );
        if ( item != m_map.end() )
            return item->second;
        else
            return std::shared_ptr<PriceBucket>();
    }

    std::shared_ptr<PriceBucket> predecessor( uint64_t price )
    {
        auto item = m_map.find( price );
        if ( item != m_map.begin() )
        {
            item--;

            std::cout << item->second->m_pricePoint << std::endl;
            return item->second;
        }
        return std::shared_ptr<PriceBucket>();
    }

    std::shared_ptr<PriceBucket> insert( bucket_set_t::value_type keyValPair )
    {
        // use the convenience of structured bindings offered by C++17
        auto [item, ok] = m_map.insert( keyValPair );

        if (ok)
            return item->second;
        else
            return std::shared_ptr<PriceBucket>();
    }

private:
    bucket_set_t m_map;
};

template < typename SetT=default_bucket_set >
class PriceBucketManager
{
public:
    // manages a set of price buckets and allows us to
    // 1. find some bucket for a specifed price-level
    // 2. find the successor bucket for the specified price-level
    // 3. find the predecessor bucket for the specified price-level
    // 4. add a new price-bucket for a given price-level

    std::shared_ptr<PriceBucket> findBucket( uint64_t price )
    {
        return m_buckets.find(price);
    }

    std::shared_ptr<PriceBucket> nextBucket( uint64_t price )
    {
        return m_buckets.successor(price);
    }

    std::shared_ptr<PriceBucket> prevBucket( uint64_t price )
    {
        return m_buckets.predecessor(price);
    }

    std::shared_ptr<PriceBucket> addBucket( uint64_t price )
    {
        auto bucket = std::make_shared<PriceBucket>(price);
        auto ret = m_buckets.insert(std::make_pair(price, bucket));
        return ret;
    }

    // SetT must be a type which supports find/successor/predecessor/insert
    // By default we use a wrapper around std::map (log n lookups). We also can
    // switch in a veb-based set type (log(log(u)) lookups) and compare the performance.
    SetT m_buckets;
};

#endif //ORDERBOOK_PRICEBUCKETMANAGER_HPP
