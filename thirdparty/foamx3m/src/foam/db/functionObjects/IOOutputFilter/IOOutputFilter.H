/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.0
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
	This file is part of foam-extend.

	foam-extend is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation, either version 3 of the License, or (at your
	option) any later version.

	foam-extend is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Class
	Foam::IOOutputFilter

Description
	IOdictionary wrapper around OutputFilter to allow them to read from
	their associated dictionaries.

SourceFiles
	IOOutputFilter.C

\*---------------------------------------------------------------------------*/

#ifndef IOOutputFilter_H
#define IOOutputFilter_H

#include "IOdictionary.H"
#include "pointFieldFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
class mapPolyMesh;


template<class OutputFilter>
class IOOutputFilter
:
	public IOdictionary,
	public OutputFilter
{
	// Private Member Functions

		// Disallow default bitwise copy construct and assignment
		IOOutputFilter(const IOOutputFilter&);
		void operator=(const IOOutputFilter&);


public:

	// Constructors

		//- Construct for given objectRegistry and dictionary
		//  Allow dictionary to be optional
		//  Allow the possibility to load fields from files
		IOOutputFilter
		(
			const word& outputFilterName,
			const objectRegistry&,
			const fileName& dictName = OutputFilter::typeName() + "Dict",
			const IOobject::readOption rOpt = IOobject::MUST_READ,
			const bool loadFromFile = false
		);


	//- Destructor
	virtual ~IOOutputFilter();


	// Member Functions

		//- Return name
		virtual const word& name() const
		{
			return IOdictionary::name();
		}

		//- Read the probes
		virtual bool read();

		//- Sample and write
		virtual void write();

		//- Update for changes of mesh
		virtual void updateMesh(const mapPolyMesh& mpm)
		{
			read();
			OutputFilter::updateMesh(mpm);
		}

		//- Update for changes of mesh
		virtual void movePoints(const pointField& points)
		{
			read();
			OutputFilter::movePoints(points);
		}
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#	include "IOOutputFilter.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
