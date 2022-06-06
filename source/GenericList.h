#if !defined( __GENERICLIST_H__ )
#define __GENERICLIST_H__
//o-----------------------------------------------------------------------------------------------o
//|	File	-	GenericList.h
//|	Date	-	2021-05-04
//o-----------------------------------------------------------------------------------------------o
//|	Purpose	-	Created as a counterpart of CDataList, based on std::list this class will behave
//|						as expected when removing records in the middle of the array.
//|
//|			-	Version Historyz
//|
//|					1.0		 		2021-05-04
//|					Initial implementation.
//o-----------------------------------------------------------------------------------------------o
#include <iostream>
#include <cstdint>
#include <unordered_set>
#include <algorithm>
//===================================================================
// GenericList
//===================================================================
// Why this design can lead to issues
// It stores iterators in an array (for push/pop).  However
// if any object is deleted, there is no guarantee that the existing iterators
// remain valid.  One may assume , because of a list, that each iterator is independent.
// However, the implementation is not defined, and the spec does not guarantee itererator
// validity outside the scope of use.  On the note of implementation, Soustrap has stated
// that one shoud almost always use vector versus list, as in most cases (due to implementation)
// it is faster.  This can be reimplemented if one assumes: 1. it only is a list of pointers, which it seems to be,
// and 2: one does not have duplicates stored.  If those are valid, this class can be reimpemented without storing
// iterators outside the working scope
template <typename T>
class GenericList
{
private:
	using GENERICLIST = std::list<T> ;
	using GENERICLIST_ITERATOR = typename std::list<T>::iterator;
	using GENERICLIST_CITERATOR = typename std::list<T>::const_iterator ;

	GENERICLIST	objData;
	GENERICLIST_ITERATOR	objIterator;
	std::vector<GENERICLIST_ITERATOR>	objIteratorBackup;

	//===================================================================
	auto FindEntry(T toFind) -> GENERICLIST_ITERATOR {
		return std::find(objData.begin(), objData.end(), toFind);
	}

	//===================================================================
	auto Begin() -> bool {
		return (objIterator == objData.begin());
	}
public:
	//===================================================================
	GenericList(){
		objData.resize(0);
		objIteratorBackup.resize(0);
		objIterator = objData.end();
	}

	//===================================================================
	~GenericList() = default ;

	//===================================================================
	auto container() const -> const std::list<T>& {
		return objData ;
	}
	//===================================================================
	auto container()  ->  std::list<T>& {
		return objData ;
	}
	//===================================================================
	auto GetCurrent() -> T {
		T rvalue = nullptr;
		if( objIterator != objData.end() )
		{
			rvalue = (*objIterator);
		}
		return rvalue;
	}
	//===================================================================
	auto First() -> T {
		objIterator = objData.end();
		return Next();
	}
	//===================================================================
	auto Next() -> T {
		T rvalue = nullptr;
		if( !Begin() )
		{
			--objIterator;
			rvalue = (*objIterator);
		}
		else
		{
			objIterator = objData.end();
		}
		return rvalue;
	}
	//===================================================================
	auto Finished() ->bool {
		return (objIterator == objData.end());
	}
	//===================================================================

	auto Num() const ->size_t {
		return objData.size();
	}
	//===================================================================
	auto Clear() ->void {
		objData.clear();
	}
	//===================================================================
	auto Add( T toAdd, bool checkForExisting = true ) ->bool{
		if( checkForExisting && FindEntry( toAdd ) != objData.end() )
		{
			return false;
		}

		const bool updateCounter = (objIterator == objData.end());
		objData.push_back(toAdd);
		if( updateCounter )
		{
			objIterator = objData.end();
		}

		return true;
	}
	//===================================================================
	auto AddInFront( T toAdd, bool checkForExisting = true ) ->bool {
		if( checkForExisting && FindEntry( toAdd ) != objData.end() )
		{
			return false;
		}

		const bool updateCounter = (objIterator == objData.end());
		objData.push_front(toAdd);
		if( updateCounter )
		{
			objIterator = objData.end();
		}

		return true;
	}
	//===================================================================
	auto Remove( T toRemove, bool handleAlloc = false ) ->bool {
		GENERICLIST_ITERATOR rIter = FindEntry( toRemove );
		if( rIter != objData.end() )
		{
			if( objIterator != objData.end() && rIter == objIterator )
			{
				++objIterator;
			}

			for( size_t q = 0; q < objIteratorBackup.size(); ++q )
			{
				if( objIteratorBackup[q] != objData.end() && rIter == objIteratorBackup[q] )
				{
					++objIteratorBackup[q];
				}
			}
			objData.erase(rIter);

			if (handleAlloc)
			{
				delete toRemove;
			}
			return true;
		}
		return false;
	}
	//===================================================================
	auto Pop() -> void {
		if( !objIteratorBackup.empty() )
		{
			objIterator = objIteratorBackup.back();
			objIteratorBackup.pop_back();
		}
		else
		{
			objIterator = objData.end();
		}
	}
	//===================================================================
	auto Push() -> void {
		objIteratorBackup.push_back(objIterator);
	}
	//===================================================================
	auto Reverse() -> void {
		objData.reverse();
	}
	//===================================================================
	auto Sort() -> void {
		objData.sort();
	}
	//===================================================================
	auto Sort( bool Comparer( T one, T two )) -> void {
		objData.sort(Comparer);
	}
};

//===================================================================
// RegionSerialList
//===================================================================
// Why is this a template given its use in uox3?
template <typename T>
class RegionSerialList
{
private:
	using REGIONSERIALLIST =  std::unordered_set<T> ;
	using REGIONSERIALLIST_ITERATOR = typename std::unordered_set<T>::iterator;
	using REGIONSERIALLIST_CITERATOR =  typename std::unordered_set<T>::const_iterator	;

	REGIONSERIALLIST					objSerials;
	REGIONSERIALLIST_ITERATOR		objIterator;
	std::pair<REGIONSERIALLIST_ITERATOR, bool> insertResult;

public:
	//===================================================================
	RegionSerialList()=default ;
	//===================================================================
	~RegionSerialList() = default ;

	//===================================================================
	auto Add( T toAdd )->bool {
		insertResult = objSerials.insert( toAdd );
		return insertResult.second;
	}
	//===================================================================
	auto Remove( T toRemove )->size_t{
		return objSerials.erase( toRemove );
	}
};

#endif // __GENERICLIST_H__

