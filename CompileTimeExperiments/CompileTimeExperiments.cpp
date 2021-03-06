// CompileTimeExperiments.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <atomic>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <tuple>
#include <any>
#include <utility>
#include <algorithm>
#include <typeinfo>

#include "TypeUtilities.h"

struct EntityType1 {
    std::string GetTypeName() { return "EntityType1"; }
};

struct EntityType2 {
    std::string GetTypeName() { return "EntityType2"; }
};

struct EntityType3 {
    std::string GetTypeName() { return "EntityType3"; }
};

struct EntityType4 {
    std::string GetTypeName() { return "EntityType4"; }
};

struct EntityType5 {
    std::string GetTypeName() { return "EntityType5"; }
};

struct EntityType6 {
    std::string GetTypeName() { return "EntityType6"; }
};

struct EntityType7 {
    std::string GetTypeName() { return "EntityType7"; }
};

struct EntityType8 {
    std::string GetTypeName() { return "EntityType8"; }
};

struct EntityType9 {
    std::string GetTypeName() { return "EntityType9"; }
};

template<typename Type>
std::string GetTypeName(Type type) {
    return type.GetTypeName();
}

namespace ECS {

    template<typename EntityTypelist>
    struct EntityManager {

        std::atomic<int> nextEntityId = 0;

        std::vector<std::any> m_entities;

        template<typename ...Ts>
        using TupleOfVectors = std::tuple<std::vector<Ts> ...>;

        using VectorTypes = TypeUtilities::Convert<EntityTypelist, TupleOfVectors>;

        VectorTypes m_entityTypeVectors;



        template<typename EntityType>
        size_t AddEntity(EntityType entity) {
            using VectorType = std::vector<EntityType>;
            size_t entityId = nextEntityId.fetch_add(1);
            m_entities.emplace_back(entity);

            std::get<VectorType>(m_entityTypeVectors).emplace_back(entity);

            return entityId;
        }

        template <typename EntityType>
        std::vector<EntityType> GetEntities() {
            using VectorType = std::vector<EntityType>;
            return std::get<VectorType>(m_entityTypeVectors);
        }

    };

}


int main()
{
    EntityType1 type1;
    EntityType2 type2;
    EntityType3 type3;
    EntityType4 type4;
    EntityType5 type5;
    EntityType6 type6;
    EntityType7 type7;
    EntityType8 type8;

    using EntityTypes = TypeUtilities::Typelist<EntityType1, EntityType2, EntityType3>;
    using EntityTypes2 = TypeUtilities::Typelist<EntityType5, EntityType6>;
    //using EntityTypes3 = Types::Typelist<EntityType7, EntityType8>;
    using EntityTypes3 = TypeUtilities::Typelist<EntityType7, EntityType8, EntityType5, EntityType3, EntityType1, EntityType3>;

    std::cout << "Types::ContainsAnyOf(EntityTypes2{}, EntityTypes{}) = " << TypeUtilities::ContainsAnyOf(EntityTypes2{}, EntityTypes{}) << "\n";

    using ConcatedTypes = TypeUtilities::Concat<EntityTypes, EntityTypes2>;
    TypeUtilities::PrintTypesInTypelist(ConcatedTypes{}, "ConcatedTypes:");
    std::cout << "\n";

    using TypeListHierarchy = TypeUtilities::Typelist<EntityTypes, EntityTypes2, EntityType4, EntityTypes3>;
    TypeUtilities::PrintTypesInTypelist(TypeListHierarchy{}, "TypeListHierarchy:");
    std::cout << "\n";

    using FlattenedHierarchy = TypeUtilities::Flatten<TypeListHierarchy>;
    TypeUtilities::PrintTypesInTypelist(FlattenedHierarchy{}, "FlattenedHierarchy:");
    std::cout << "\n";

    using FlattenedEntityTypes = TypeUtilities::Flatten<EntityTypes, EntityTypes2, EntityType4, EntityTypes3>;
    TypeUtilities::PrintTypesInTypelist(FlattenedEntityTypes{}, "FlattenedEntityTypes:");
    std::cout << "\n";


    std::string var = TypeUtilities::Impl::FlattenImpl<EntityTypes, EntityTypes2, EntityType4, EntityTypes3>::value;
    std::cout << "var = " << var << "\n";
    std::cout << "\n";
    using tuple = TypeUtilities::Convert<FlattenedEntityTypes, std::tuple>;
    tuple tupleVar;

    size_t tupleSize = std::tuple_size<tuple>::value;

    using EntityType1Dependencies = TypeUtilities::TypeDependencies<EntityType1>;
    using EntityType2Dependencies = TypeUtilities::TypeDependencies<EntityType2, EntityType1>;
    using EntityType3Dependencies = TypeUtilities::TypeDependencies<EntityType3, EntityType1>;
    using EntityType4Dependencies = TypeUtilities::TypeDependencies<EntityType4, EntityType2, EntityType3>;
    using EntityType5Dependencies = TypeUtilities::TypeDependencies<EntityType5, EntityType2>;

    TypeUtilities::PrintTypes(EntityType1Dependencies{}, "EntityType1Dependencies:");
    std::cout << "\n";

    std::cout << "TypeUtilities::Impl::IsDependentImpl<EntityType4Dependencies, EntityType2>::value = " << TypeUtilities::Impl::IsDependentImpl<EntityType4Dependencies, EntityType2>::value << "\n\n";

    //TypeUtilities::PrintTypes(TypeUtilities::Impl::IsDependentImpl<EntityType4Dependencies, EntityType2>::dependencies{}, "IsDependentImpl::dependencies");

    using EntityTypeDependencies = TypeUtilities::Typelist<EntityType5Dependencies, EntityType3Dependencies, EntityType2Dependencies, EntityType4Dependencies, EntityType1Dependencies>;
    TypeUtilities::PrintTypesInTypelist(EntityTypeDependencies{}, "EntityTypeDependencies:");
    std::cout << "\n";

    std::cout << "TypeUtilities::Impl::GetDependentTypesImpl<EntityType2, EntityTypeDependencies>::debug = " << TypeUtilities::Impl::GetDependentTypesImpl<EntityType2, EntityType1Dependencies>::debug << "\n";
    std::cout << "\n";

    using DependentTypes = TypeUtilities::Impl::GetDependentTypesImpl<EntityType2, EntityTypeDependencies>::type;
    TypeUtilities::PrintTypes(DependentTypes{}, "DependentTypes:");
    std::cout << "\n";

    std::cout << "TypeUtilities::Impl::GetDependentTypesImpl<EntityType2, EntityTypeDependencies>::value = " << TypeUtilities::Impl::GetDependentTypesImpl<EntityType2, EntityTypeDependencies>::value << "\n";
    std::cout << "\n";

    using PrimaryTypes = typename TypeUtilities::GetPrimaryTypes<EntityTypeDependencies>;
    TypeUtilities::PrintTypesInTypelist(PrimaryTypes{}, "PrimaryTypes:");
    std::cout << "\n";

    using RemainingTypeDependencies1 = TypeUtilities::Impl::LeastDependentImpl<EntityTypeDependencies, PrimaryTypes>::remainingTypeDependencies;
    TypeUtilities::PrintTypesInTypelist(RemainingTypeDependencies1{}, "RemainingTypeDependencies1");
    std::cout << "\n";

    using LeastDependentType1 = TypeUtilities::Impl::LeastDependentImpl<RemainingTypeDependencies1, PrimaryTypes>::type;
    std::cout << "typeid(LeastDependentType1).name() = " << typeid(LeastDependentType1).name() << "\n";
    std::cout << "\n";

    using RemainingTypeDependencies2 = TypeUtilities::Impl::LeastDependentImpl<RemainingTypeDependencies1, PrimaryTypes>::remainingTypeDependencies;
    TypeUtilities::PrintTypesInTypelist(RemainingTypeDependencies2{}, "RemainingTypeDependencies2");
    std::cout << "\n";

    

    using LeastDependent = TypeUtilities::Impl::LeastDependentImpl<EntityTypeDependencies, PrimaryTypes>::type;

    std::cout << "typeid(LeastDependent).name() = " << typeid(LeastDependent).name() << "\n";
    std::cout << "\n";

    using SortedTypeDependenciesLeastDependentType = typename TypeUtilities::Impl::SortTypeDependenciesImpl<EntityTypeDependencies>::leastDependent;
    std::cout << "typeid(SortedTypeDependenciesLeastDependentType).name() = " << typeid(SortedTypeDependenciesLeastDependentType).name() << "\n";
    std::cout << "\n";

    using SortedTypeDependenciesRemainder = typename TypeUtilities::Impl::SortTypeDependenciesImpl<EntityTypeDependencies>::remainder;
    TypeUtilities::PrintTypesInTypelist(SortedTypeDependenciesRemainder{}, "SortedTypeDependenciesRemainder:");
    std::cout << "\n";

    using RemovedLeastDependent = TypeUtilities::Impl::RemoveTypeImpl<SortedTypeDependenciesLeastDependentType, EntityTypeDependencies>::type;
    TypeUtilities::PrintTypesInTypelist(RemovedLeastDependent{}, "RemovedLeastDependent:");
    std::cout << "\n";

    using SortedTypeDependencies = TypeUtilities::SortTypeDependencies<EntityTypeDependencies>;
    TypeUtilities::PrintTypesInTypelist(SortedTypeDependencies{}, "SortedTypeDependencies:");
    std::cout << "\n";

    


    using Typelist0 = TypeUtilities::Typelist<EntityType2>;

    using Typelist1 = TypeUtilities::PushFront<Typelist0, EntityType1>;

    using Typelist2 = TypeUtilities::PushBack<Typelist1, EntityType3>;

    //using Typelist3 = Types::RemoveType<Typelist2, EntityType1>;
    //using Typelist3 = Types::RemoveAt<1, FlattenedEntityTypes>;
    std::cout << "FlattenedEntityTypes::size() = " << FlattenedEntityTypes::size() << "\n";
    std::cout << "\n";
    //using Typelist3 = Types::Impl::RemoveAllOfImpl<100, EntityType5, FlattenedEntityTypes>::type;
    //using Typelist3 = Types::RemoveDuplicates<FlattenedEntityTypes>;
    using Typelist3a = TypeUtilities::ReplaceFirst<EntityType3, EntityType5, FlattenedEntityTypes>::type;
    using Typelist3 = TypeUtilities::RemoveDuplicates<Typelist3a>;
    //using Typelist4 = Types::Impl::ReplaceTypeImpl<EntityType1, EntityType2, FlattenedEntityTypes>::type;

    TypeUtilities::PrintTypesInTypelist(FlattenedEntityTypes{}, "FlattenedEntityTypes Types: ");
    std::cout << "\n";
    TypeUtilities::PrintTypesInTypelist(Typelist3a{}, "Typelist3a Types: ");
    std::cout << "\n";
    TypeUtilities::PrintTypesInTypelist(Typelist3{}, "Typelist3 Types: ");
    std::cout << "\n";

    std::cout << "Typelist3::size() = " << Typelist3::size() << "\n";
    std::cout << "\n";
    //std::cout << "Typelist4::size() = " << Typelist4::size() << "\n";
    std::cout << "Types::Impl::RemoveDuplicatesImpl<FlattenedEntityTypes>::debug = " << TypeUtilities::Impl::RemoveDuplicatesImpl<FlattenedEntityTypes>::debug << "\n";
    std::cout << "\n";

    std::cout << "Types::IndexOfFunction<EntityType3, FlattenedEntityTypes>::value = " << TypeUtilities::IndexOf<EntityType3, FlattenedEntityTypes>() << "\n";
    std::cout << "\n";
    std::cout << "Types::IndexOfFunction<EntityType9, FlattenedEntityTypes>::value = " << TypeUtilities::IndexOf<EntityType9, FlattenedEntityTypes>() << "\n";
    std::cout << "\n";

    if (TypeUtilities::Contains<EntityType9, FlattenedEntityTypes>()) {
        std::cout << "FlattendedEntityTypes contains EntityType9\n";
        std::cout << "\n";
    }
    else {
        std::cout << "FlattendedEntityTypes does not contain EntityType9\n";
        std::cout << "\n";
    }

    ECS::EntityManager<Typelist3> entityManager;

    //entityManager.AddEntity(type1);
    entityManager.AddEntity(type2);
    entityManager.AddEntity(type3);
    entityManager.AddEntity(type4);
    entityManager.AddEntity(type5);
    entityManager.AddEntity(type6);
    entityManager.AddEntity(type7);
    //entityManager.AddEntity(type8);

    for (auto stdAnyEntity : entityManager.m_entities) {
        std::cout << stdAnyEntity.type().name() << "\n";
    }

    system("pause");

    return 0;
}

