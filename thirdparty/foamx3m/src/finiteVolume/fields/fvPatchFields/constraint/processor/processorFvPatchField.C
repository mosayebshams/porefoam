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

\*---------------------------------------------------------------------------*/

#include "processorFvPatchField.H"
#include "processorFvPatch.H"
#include "IPstream.H"
#include "OPstream.H"
#include "transformField.H"
#include "coeffFields.H"
#include "demandDrivenData.H"

// * * * * * * * * * * * * * * * * Constructors * * * * * * * * * * * * * * //

template<class Type>
Foam::processorFvPatchField<Type>::processorFvPatchField
(
	const fvPatch& p,
	const DimensionedField<Type, volMesh>& iF
)
:
	coupledFvPatchField<Type>(p, iF),
	procPatch_(refCast<const processorFvPatch>(p)),
	outstandingSendRequest_(-1),
	outstandingRecvRequest_(-1),
	sendBuf_(0),
	receiveBuf_(0),
	scalarSendBuf_(0),
	scalarReceiveBuf_(0)
{}


template<class Type>
Foam::processorFvPatchField<Type>::processorFvPatchField
(
	const fvPatch& p,
	const DimensionedField<Type, volMesh>& iF,
	const Field<Type>& f
)
:
	coupledFvPatchField<Type>(p, iF, f),
	procPatch_(refCast<const processorFvPatch>(p)),
	outstandingSendRequest_(-1),
	outstandingRecvRequest_(-1),
	sendBuf_(0),
	receiveBuf_(0),
	scalarSendBuf_(0),
	scalarReceiveBuf_(0)
{}


// Construct by mapping given processorFvPatchField<Type>
template<class Type>
Foam::processorFvPatchField<Type>::processorFvPatchField
(
	const processorFvPatchField<Type>& ptf,
	const fvPatch& p,
	const DimensionedField<Type, volMesh>& iF,
	const fvPatchFieldMapper& mapper
)
:
	coupledFvPatchField<Type>(ptf, p, iF, mapper),
	procPatch_(refCast<const processorFvPatch>(p)),
	outstandingSendRequest_(-1),
	outstandingRecvRequest_(-1),
	sendBuf_(0),
	receiveBuf_(0),
	scalarSendBuf_(0),
	scalarReceiveBuf_(0)
{
	if (!isType<processorFvPatch>(this->patch()))
	{
		FatalErrorIn
		(
			"processorFvPatchField<Type>::processorFvPatchField\n"
			"(\n"
			"    const processorFvPatchField<Type>& ptf,\n"
			"    const fvPatch& p,\n"
			"    const DimensionedField<Type, volMesh>& iF,\n"
			"    const fvPatchFieldMapper& mapper\n"
			")\n"
		)   << "\n	patch type '" << p.type()
			<< "' not constraint type '" << typeName << "'"
			<< "\n	for patch " << p.name()
			<< " of field " << this->dimensionedInternalField().name()
			<< " in file " << this->dimensionedInternalField().objectPath()
			<< exit(FatalIOError);
	}
}


template<class Type>
Foam::processorFvPatchField<Type>::processorFvPatchField
(
	const fvPatch& p,
	const DimensionedField<Type, volMesh>& iF,
	const dictionary& dict
)
:
	coupledFvPatchField<Type>(p, iF, dict),
	procPatch_(refCast<const processorFvPatch>(p)),
	outstandingSendRequest_(-1),
	outstandingRecvRequest_(-1),
	sendBuf_(0),
	receiveBuf_(0),
	scalarSendBuf_(0),
	scalarReceiveBuf_(0)
{
	if (!isType<processorFvPatch>(p))
	{
		FatalIOErrorIn
		(
			"processorFvPatchField<Type>::processorFvPatchField\n"
			"(\n"
			"    const fvPatch& p,\n"
			"    const Field<Type>& field,\n"
			"    const dictionary& dict\n"
			")\n",
			dict
		)   << "\n	patch type '" << p.type()
			<< "' not constraint type '" << typeName << "'"
			<< "\n	for patch " << p.name()
			<< " of field " << this->dimensionedInternalField().name()
			<< " in file " << this->dimensionedInternalField().objectPath()
			<< exit(FatalIOError);
	}
}


template<class Type>
Foam::processorFvPatchField<Type>::processorFvPatchField
(
	const processorFvPatchField<Type>& ptf
)
:
	processorLduInterfaceField(),
	coupledFvPatchField<Type>(ptf),
	procPatch_(refCast<const processorFvPatch>(ptf.patch())),
	outstandingSendRequest_(-1),
	outstandingRecvRequest_(-1),
	sendBuf_(0),
	receiveBuf_(0),
	scalarSendBuf_(0),
	scalarReceiveBuf_(0)
{}


template<class Type>
Foam::processorFvPatchField<Type>::processorFvPatchField
(
	const processorFvPatchField<Type>& ptf,
	const DimensionedField<Type, volMesh>& iF
)
:
	coupledFvPatchField<Type>(ptf, iF),
	procPatch_(refCast<const processorFvPatch>(ptf.patch())),
	outstandingSendRequest_(-1),
	outstandingRecvRequest_(-1),
	sendBuf_(0),
	receiveBuf_(0),
	scalarSendBuf_(0),
	scalarReceiveBuf_(0)
{

}


// * * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * //

template<class Type>
Foam::processorFvPatchField<Type>::~processorFvPatchField()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
Foam::tmp<Foam::Field<Type> >
Foam::processorFvPatchField<Type>::patchNeighbourField() const
{


	// Warning: returning own patch field, which only after update stores
	// actual neighbour data
	// HJ, 14/May/2009
	return *this;
}


template<class Type>
void Foam::processorFvPatchField<Type>::initEvaluate
(
	const Pstream::commsTypes commsType
)
{
	if (Pstream::parRun())
	{
		procPatch_.compressedSend(commsType, this->patchInternalField()());
	}
}


template<class Type>
void Foam::processorFvPatchField<Type>::evaluate
(
	const Pstream::commsTypes commsType
)
{
	if (Pstream::parRun())
	{
		procPatch_.compressedReceive<Type>(commsType, *this);

		if (doTransform())
		{
			transform(*this, procPatch_.forwardT(), *this);
		}
	}
}


template<class Type>
Foam::tmp<Foam::Field<Type> >
Foam::processorFvPatchField<Type>::snGrad() const
{
	return this->patch().deltaCoeffs()*(*this - this->patchInternalField());
}


template<class Type>
void Foam::processorFvPatchField<Type>::initInterfaceMatrixUpdate
(
	const scalarField& psiInternal,
	scalarField&,
	const lduMatrix&,
	const scalarField&,
	const direction,
	const Pstream::commsTypes commsType,
	const bool switchToLhs
) const
{
	procPatch_.compressedSend
	(
		commsType,
		this->patch().patchInternalField(psiInternal)()
	);
}


template<class Type>
void Foam::processorFvPatchField<Type>::updateInterfaceMatrix
(
	const scalarField&,
	scalarField& result,
	const lduMatrix&,
	const scalarField& coeffs,
	const direction cmpt,
	const Pstream::commsTypes commsType,
	const bool switchToLhs
) const
{
	scalarField pnf
	(
		procPatch_.compressedReceive<scalar>(commsType, this->size())()
	);

	// Transform according to the transformation tensor
	transformCoupleField(pnf, cmpt);

	// Multiply the field by coefficients and add into the result

	const unallocLabelList& faceCells = this->patch().faceCells();

	if (switchToLhs)
	{
		forAll(faceCells, elemI)
		{
			result[faceCells[elemI]] += coeffs[elemI]*pnf[elemI];
		}
	}
	else
	{
		forAll(faceCells, elemI)
		{
			result[faceCells[elemI]] -= coeffs[elemI]*pnf[elemI];
		}
	}
}


// ************************************************************************* //
