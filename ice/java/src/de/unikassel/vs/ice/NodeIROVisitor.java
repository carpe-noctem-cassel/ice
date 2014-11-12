package de.unikassel.vs.ice;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

import org.semanticweb.owlapi.model.IRI;
import org.semanticweb.owlapi.model.OWLAnnotation;
import org.semanticweb.owlapi.model.OWLAnnotationAssertionAxiom;
import org.semanticweb.owlapi.model.OWLAnnotationProperty;
import org.semanticweb.owlapi.model.OWLAnnotationPropertyDomainAxiom;
import org.semanticweb.owlapi.model.OWLAnnotationPropertyRangeAxiom;
import org.semanticweb.owlapi.model.OWLAnonymousIndividual;
import org.semanticweb.owlapi.model.OWLAsymmetricObjectPropertyAxiom;
import org.semanticweb.owlapi.model.OWLClass;
import org.semanticweb.owlapi.model.OWLClassAssertionAxiom;
import org.semanticweb.owlapi.model.OWLClassExpression;
import org.semanticweb.owlapi.model.OWLDataAllValuesFrom;
import org.semanticweb.owlapi.model.OWLDataComplementOf;
import org.semanticweb.owlapi.model.OWLDataExactCardinality;
import org.semanticweb.owlapi.model.OWLDataFactory;
import org.semanticweb.owlapi.model.OWLDataHasValue;
import org.semanticweb.owlapi.model.OWLDataIntersectionOf;
import org.semanticweb.owlapi.model.OWLDataMaxCardinality;
import org.semanticweb.owlapi.model.OWLDataMinCardinality;
import org.semanticweb.owlapi.model.OWLDataOneOf;
import org.semanticweb.owlapi.model.OWLDataProperty;
import org.semanticweb.owlapi.model.OWLDataPropertyAssertionAxiom;
import org.semanticweb.owlapi.model.OWLDataPropertyDomainAxiom;
import org.semanticweb.owlapi.model.OWLDataPropertyRangeAxiom;
import org.semanticweb.owlapi.model.OWLDataSomeValuesFrom;
import org.semanticweb.owlapi.model.OWLDataUnionOf;
import org.semanticweb.owlapi.model.OWLDatatype;
import org.semanticweb.owlapi.model.OWLDatatypeDefinitionAxiom;
import org.semanticweb.owlapi.model.OWLDatatypeRestriction;
import org.semanticweb.owlapi.model.OWLDeclarationAxiom;
import org.semanticweb.owlapi.model.OWLDifferentIndividualsAxiom;
import org.semanticweb.owlapi.model.OWLDisjointClassesAxiom;
import org.semanticweb.owlapi.model.OWLDisjointDataPropertiesAxiom;
import org.semanticweb.owlapi.model.OWLDisjointObjectPropertiesAxiom;
import org.semanticweb.owlapi.model.OWLDisjointUnionAxiom;
import org.semanticweb.owlapi.model.OWLEquivalentClassesAxiom;
import org.semanticweb.owlapi.model.OWLEquivalentDataPropertiesAxiom;
import org.semanticweb.owlapi.model.OWLEquivalentObjectPropertiesAxiom;
import org.semanticweb.owlapi.model.OWLFacetRestriction;
import org.semanticweb.owlapi.model.OWLFunctionalDataPropertyAxiom;
import org.semanticweb.owlapi.model.OWLFunctionalObjectPropertyAxiom;
import org.semanticweb.owlapi.model.OWLHasKeyAxiom;
import org.semanticweb.owlapi.model.OWLInverseFunctionalObjectPropertyAxiom;
import org.semanticweb.owlapi.model.OWLInverseObjectPropertiesAxiom;
import org.semanticweb.owlapi.model.OWLIrreflexiveObjectPropertyAxiom;
import org.semanticweb.owlapi.model.OWLLiteral;
import org.semanticweb.owlapi.model.OWLNamedIndividual;
import org.semanticweb.owlapi.model.OWLNegativeDataPropertyAssertionAxiom;
import org.semanticweb.owlapi.model.OWLNegativeObjectPropertyAssertionAxiom;
import org.semanticweb.owlapi.model.OWLObjectAllValuesFrom;
import org.semanticweb.owlapi.model.OWLObjectComplementOf;
import org.semanticweb.owlapi.model.OWLObjectExactCardinality;
import org.semanticweb.owlapi.model.OWLObjectHasSelf;
import org.semanticweb.owlapi.model.OWLObjectHasValue;
import org.semanticweb.owlapi.model.OWLObjectIntersectionOf;
import org.semanticweb.owlapi.model.OWLObjectInverseOf;
import org.semanticweb.owlapi.model.OWLObjectMaxCardinality;
import org.semanticweb.owlapi.model.OWLObjectMinCardinality;
import org.semanticweb.owlapi.model.OWLObjectOneOf;
import org.semanticweb.owlapi.model.OWLObjectProperty;
import org.semanticweb.owlapi.model.OWLObjectPropertyAssertionAxiom;
import org.semanticweb.owlapi.model.OWLObjectPropertyDomainAxiom;
import org.semanticweb.owlapi.model.OWLObjectPropertyExpression;
import org.semanticweb.owlapi.model.OWLObjectPropertyRangeAxiom;
import org.semanticweb.owlapi.model.OWLObjectSomeValuesFrom;
import org.semanticweb.owlapi.model.OWLObjectUnionOf;
import org.semanticweb.owlapi.model.OWLOntology;
import org.semanticweb.owlapi.model.OWLReflexiveObjectPropertyAxiom;
import org.semanticweb.owlapi.model.OWLSameIndividualAxiom;
import org.semanticweb.owlapi.model.OWLSubAnnotationPropertyOfAxiom;
import org.semanticweb.owlapi.model.OWLSubClassOfAxiom;
import org.semanticweb.owlapi.model.OWLSubDataPropertyOfAxiom;
import org.semanticweb.owlapi.model.OWLSubObjectPropertyOfAxiom;
import org.semanticweb.owlapi.model.OWLSubPropertyChainOfAxiom;
import org.semanticweb.owlapi.model.OWLSymmetricObjectPropertyAxiom;
import org.semanticweb.owlapi.model.OWLTransitiveObjectPropertyAxiom;
import org.semanticweb.owlapi.model.SWRLBuiltInAtom;
import org.semanticweb.owlapi.model.SWRLClassAtom;
import org.semanticweb.owlapi.model.SWRLDataPropertyAtom;
import org.semanticweb.owlapi.model.SWRLDataRangeAtom;
import org.semanticweb.owlapi.model.SWRLDifferentIndividualsAtom;
import org.semanticweb.owlapi.model.SWRLIndividualArgument;
import org.semanticweb.owlapi.model.SWRLLiteralArgument;
import org.semanticweb.owlapi.model.SWRLObjectPropertyAtom;
import org.semanticweb.owlapi.model.SWRLRule;
import org.semanticweb.owlapi.model.SWRLSameIndividualAtom;
import org.semanticweb.owlapi.model.SWRLVariable;
import org.semanticweb.owlapi.reasoner.OWLReasoner;

public class NodeIROVisitor extends IceVisitor {

	// private static final String IRO_PLACEHOLDER = "$IRO";

	enum Type {
		SOURCE_NODE, COMPUTATION_NODE, IRO_NODE, MAP_NODE, REQUIRED_STREAM, REQUIRED_MAP
	};

	private Type currentType;
	private OWLClass lastElement;
	// private OWLClass iroScope;
	// private OWLClass iroRelatedScope;
	private OWLClass currentScope;
	private OWLClass currentRepresentation;
	private OWLClass currentEntityType;
	private OWLNamedIndividual currentSystem;
	private OWLNamedIndividual currentEntity;
	private OWLNamedIndividual currentRelatedEntity;
	private String elementString;
	private boolean found;
	private List<CardinalityContainer> cardinalityContainers;

	private OWLNamedIndividual grounding;

	public NodeIROVisitor(final IceOntologyInterface p_ioi, final Set<OWLOntology> p_ontologies,
			final OWLReasoner p_reasoner, final OWLDataFactory p_dataFactory) {
		super(p_ioi, p_ontologies, p_reasoner, p_dataFactory);
	}

	public void start() {
		Set<OWLNamedIndividual> systems = this.reasoner.getInstances(this.system, true).getFlattened();

		for (OWLNamedIndividual system : systems) {
			this.readInformation(system);
		}
	}

	public List<List<String>> readInformation(OWLNamedIndividual system) {
		log("Creating IROs for system " + system);

		List<List<String>> result = new ArrayList<List<String>>();
		result.add(new ArrayList<String>());
		result.add(new ArrayList<String>());
		result.add(new ArrayList<String>());
		result.add(new ArrayList<String>());
		result.add(new ArrayList<String>());

		this.currentSystem = system;

		Set<OWLNamedIndividual> groundings = this.reasoner.getObjectPropertyValues(system, isSystemOf).getFlattened();

		for (OWLNamedIndividual grounding : groundings) {
			log(String.format("Checking grounding %s in system %s", grounding, system));
			this.grounding = grounding;

			this.cardinalityContainers = new ArrayList<NodeIROVisitor.CardinalityContainer>();
			this.currentEntity = null;
			this.currentRelatedEntity = null;
			this.currentScope = null;
			this.currentRepresentation = null;
			this.currentEntityType = null;
			this.currentType = null;
			this.sb = new StringBuffer();

			// check about entity
			Set<OWLNamedIndividual> entities = this.reasoner.getObjectPropertyValues(grounding, this.aboutEntity)
					.getFlattened();

			if (entities.size() == 1) {
				this.currentEntity = entities.iterator().next();
			} else if (entities.size() > 1) {
				// TODO
			}

			// check about related entity
			entities = this.reasoner.getObjectPropertyValues(grounding, this.aboutRelatedEntity).getFlattened();

			if (entities.size() == 1) {
				this.currentRelatedEntity = entities.iterator().next();
			} else if (entities.size() > 1) {
				// TODO
			}

			this.found = false;
			// check types
			Collection<OWLClassExpression> types = new ArrayList<OWLClassExpression>();

			for (OWLOntology ont : this.ontologies)
				types.addAll(grounding.getTypes(ont));
			// OWL API 4.0
			// types.addAll(EntitySearcher.getTypes(grounding, ont));

			for (OWLClassExpression type : types) {
				if (this.isSubClassOf(type, this.requiredStream)) {
					if (found)
						continue;

					this.found = true;
					this.currentType = Type.REQUIRED_STREAM;
				} else if (this.isSubClassOf(type, this.requiredMap)) {
					if (found)
						continue;

					this.found = true;
					this.currentType = Type.REQUIRED_MAP;
				}

				type.accept(this);
			}

			if (this.currentType == Type.REQUIRED_STREAM) {
				// #external requiredStream(system,information).
				if (this.currentEntity == null || this.currentScope == null || this.currentRepresentation == null) {
					this.log(String
							.format("Missing information description elements '%s' (entity), '%s' (scope) '%s' (representation)",
									this.currentEntity, this.currentScope, this.currentRepresentation));
				} else {
					this.elementString = this.replace("requiredStream($system,$information).\n");
					this.sb.append("#external ");
					this.sb.append(this.elementString);
				}
			} else if (this.currentType == Type.REQUIRED_MAP) {
				// #external
				// requiredMap(SYSTEM,ENTITY_TYPE,SCOPE,REPRESENTATION,ENTITY2)
				String info = this.replace("requiredMap($system,$type,$scope,$representation,$relatedEntity).\n");

				if (info.indexOf("$") >= 0) {
					this.log(String
							.format("Missing information description elements '%s' (entityType), '%s' (scope), '%s' (representation), '%s' (entity2)",
									this.currentEntityType, this.currentScope, this.currentRepresentation,
									this.currentRelatedEntity));
				} else {
					this.elementString = info;
					this.sb.append("#external ");
					this.sb.append(this.elementString);
				}
			}

			// check metadata
			Set<OWLNamedIndividual> metadatas = this.reasoner.getObjectPropertyValues(grounding, this.hasMetadata)
					.getFlattened();

			for (OWLNamedIndividual metadata : metadatas) {
				metadata.accept(this);
			}

			if (this.currentType == null || this.elementString == null) {
				continue;
			}

			if (this.elementString.indexOf("$") >= 0 | this.sb.indexOf("$") >= 0) {
				this.log(String.format("'%s' or grounding '%s' contains unreplaced elements",
						this.elementString.replace("\n", ""), this.sb.toString().replace("\n", "||")));
				continue;
			}

			// TODO check cardinality containers

			this.log(String.format("Adding asp element '%s' of type '%s'", this.grounding.toString(),
					this.currentType.toString()));

			result.get(0).add(this.currentType.toString());
			result.get(1).add(this.iRIShortName(this.grounding.getIRI()));
			result.get(2).add(this.elementString);
			result.get(3).add(this.sb.toString());

			this.sb = new StringBuffer();
			this.grounding.accept(this);

			result.get(4).add(this.sb.toString());
		}

		return result;
	}

	@Override
	public void visit(OWLNamedIndividual individual) {

		Set<OWLNamedIndividual> groundings = this.reasoner.getObjectPropertyValues(individual, this.hasGrounding)
				.getFlattened();

		if (groundings.size() != 1) {
			log("Wrong size of hasGrounding properties " + groundings.size() + " " + individual + " will be skipped.");
		} else {
			OWLNamedIndividual ind = groundings.iterator().next();

			Collection<OWLClassExpression> types = new ArrayList<OWLClassExpression>();

			for (OWLOntology ont : this.ontologies)
				types.addAll(ind.getTypes(ont));
			// OWL API 4.0
			// types.addAll(EntitySearcher.getTypes(ind, ont));

			int count = 0;

			for (OWLClassExpression type : types) {
				if (this.isSubClassOf(type, this.groundingOWLClass))
					++count;
			}

			if (count != 1) {
				log("Individual " + individual + " is not grounding and will be skipped.");
				return;
			}

			Set<OWLLiteral> literals = this.reasoner.getDataPropertyValues(individual, this.hasMetadataValue);
			String value;

			if (literals.size() != 1) {
				log("Wrong size of hasMetadataValue properties " + literals.size() + " " + individual
						+ " will be skipped.");
				value = null;
			} else {
				value = literals.iterator().next().getLiteral();
			}

			Set<OWLLiteral> literals2 = this.reasoner.getDataPropertyValues(individual, this.hasMetadataValue2);
			String value2;

			if (literals2.size() != 1) {
				log("Wrong size of hasMetadataValue2 properties " + literals2.size() + " " + individual
						+ " will be skipped.");
				value2 = null;
			} else {
				value2 = literals2.iterator().next().getLiteral();
			}

			literals = this.reasoner.getDataPropertyValues(ind, this.hasGroundingValue);

			for (OWLLiteral lit : literals) {
				String pattern = this.replace(lit.getLiteral());

				if (value != null)
					pattern = pattern.replace("$value1", value);
				if (value2 != null)
					pattern = pattern.replace("$value2", value2);

				sb.append(pattern + "\n");
			}

			literals = this.reasoner.getDataPropertyValues(ind, this.hasConfiguration);

			for (OWLLiteral lit : literals) {
				sb.append(lit.getLiteral() + "\n");
			}
		}
	}

	@Override
	public void visit(OWLClass ce) {
		OWLClass lastNode = this.lastElement;
		List<OWLSubClassOfAxiom> others = new ArrayList<OWLSubClassOfAxiom>();
		boolean doLater = false;
		for (OWLOntology ont : this.ontologies) {
			for (OWLSubClassOfAxiom ax : ont.getSubClassAxiomsForSubClass(ce)) {
				if (this.isSubClassOf(ax.getSuperClass(), this.sourceNode)) {
					if (found)
						continue;

					if (this.currentEntity == null) {
						this.log(String.format("No entity specified for source node %s, skipping node", this.grounding));
						continue;
					}

					this.found = true;
					this.currentType = Type.SOURCE_NODE;
					this.lastElement = ce;

					// #external nodeTemplate(system1,node1,any).
					StringBuffer sb = new StringBuffer();
					sb.append("sourceNode(");
					sb.append(this.iRIShortName(this.currentSystem.getIRI()));
					sb.append(",");
					sb.append(this.iRIShortName(this.grounding.getIRI()));
					sb.append(",");
					sb.append(this.iRIShortName(this.currentEntity.getIRI()));
					sb.append(").\n");

					this.elementString = sb.toString();
					this.sb.append("#external ");
					this.sb.append(this.elementString);
				} else if (this.isSubClassOf(ax.getSuperClass(), this.computationNode)) {
					if (found)
						continue;

					this.found = true;
					this.currentType = Type.COMPUTATION_NODE;
					this.lastElement = ce;

					// #external nodeTemplate(system1,node1,any).
					StringBuffer sb = new StringBuffer();
					sb.append("nodeTemplate(");
					sb.append(this.iRIShortName(this.currentSystem.getIRI()));
					sb.append(",");
					sb.append(this.iRIShortName(this.grounding.getIRI()));
					sb.append(",");
					sb.append((this.currentEntity != null) ? this.iRIShortName(this.currentEntity.getIRI()) : "any");
					sb.append(").\n");

					this.elementString = sb.toString();
					this.sb.append("#external ");
					this.sb.append(this.elementString);
				} else if (this.isSubClassOf(ax.getSuperClass(), this.iro)) {
					if (found)
						continue;

					this.found = true;
					this.currentType = Type.IRO_NODE;
					this.lastElement = ce;
					doLater = true;
				} else if (this.isSubClassOf(ax.getSuperClass(), this.mapNode)) {
					if (found)
						continue;

					this.found = true;
					this.currentType = Type.MAP_NODE;
					this.lastElement = ce;
					doLater = true;
				} else if (this.isSubClassOf(ax.getSuperClass(), this.entityScope)) {
					this.currentScope = ce;
				} else {
					// log(ax);
					others.add(ax);
				}
			}
		}

		for (OWLSubClassOfAxiom ax : others) {
			// others
			ax.getSuperClass().accept(this);
		}

		if (doLater && this.currentType == Type.IRO_NODE) {
			// #external iro(system1,coords2Wgs84,any,position).
			String string = this.replace("iro($system,%element,$entity,$relatedEntity).\n");

			// StringBuffer sb = new StringBuffer();
			// sb.append("iro(");
			// sb.append(this.iRIShortName(this.system.getIRI()));
			// sb.append(",");
			// sb.append(this.iRIShortName(this.grounding.getIRI()));
			// sb.append(",");
			// sb.append((this.currentEntity != null) ?
			// this.iRIShortName(this.currentEntity.getIRI()) : "any");
			// sb.append(",");
			// sb.append(this.iRIShortName(this.iroScope.getIRI()));
			//
			// if (this.iroRelatedScope != null) {
			// sb.append(",");
			// sb.append((this.currentEntity != null) ?
			// this.iRIShortName(this.currentEntity.getIRI()) : "any");
			// sb.append(",");
			// sb.append(this.iRIShortName(this.iroRelatedScope.getIRI()));
			// }
			//
			// sb.append(").\n");

			this.elementString = string;
			this.sb.append("#external ");
			this.sb.append(this.elementString);
		} else if (doLater && this.currentType == Type.MAP_NODE) {
			// mapNodeTemplate(SYSTEM,NODE,ENTITY_TYPE)
			String node = this.replace("mapNodeTemplate($system,$element,$type).\n");

			this.elementString = node;
			this.sb.append("#external ");
			this.sb.append(this.elementString);
		}

		this.lastElement = lastNode;
	}

	@Override
	public void visit(OWLObjectExactCardinality ce) {
		int cardinality = ce.getCardinality();
		this.processNodeStreamRelation(ce.getProperty(), ce.getFiller(), cardinality, cardinality);
	}

	public void visit(OWLObjectSomeValuesFrom ce) {

		if (ce.getProperty().equals(isStreamOf)) {
			if (ce.getFiller().isAnonymous()) {
				ce.getFiller().accept(this);
			} else {
				this.currentScope = ce.getFiller().asOWLClass();
			}
		} else if (ce.getProperty().equals(isMapOf)) {
			if (ce.getFiller().isAnonymous()) {
				ce.getFiller().accept(this);
			} else {
				this.currentScope = ce.getFiller().asOWLClass();
			}
		} else if (ce.getProperty().equals(hasRepresentation)) {
			if (ce.getFiller().isAnonymous()) {
				log("Anonymous Representation? " + ce.getFiller());
			} else {
				this.currentRepresentation = ce.getFiller().asOWLClass();
			}
		} else if (ce.getProperty().equals(aboutEntity)) {
			this.currentEntityType = ce.getFiller().asOWLClass();
		} else if (ce.getProperty().equals(hasStreamMetadata)) {
			// currently ignored
		} else if (ce.getProperty().equals(improveInformationMetadata)) {
			// currently ignored
		} else if (ce.getProperty().equals(impairInformationMetadata)) {
			// currently ignored
		} else {
			if (false == this.processNodeStreamRelation(ce.getProperty(), ce.getFiller(),
					this.ioi.getSomeMinCardinality(), this.ioi.getSomeMaxCardinality()))
				log("Unknown OWLObjectSomeValuesFrom " + ce);
		}
	}

	@Override
	public void visit(OWLObjectMinCardinality ce) {
		int min = ce.getCardinality();
		this.processNodeStreamRelation(ce.getProperty(), ce.getFiller(), min, -1);
	}

	@Override
	public void visit(OWLObjectMaxCardinality ce) {
		int max = ce.getCardinality();
		this.processNodeStreamRelation(ce.getProperty(), ce.getFiller(), -1, max);
	}

	@Override
	public void visit(OWLObjectIntersectionOf ce) {
		TreeSet<OWLClassExpression> treeSet = new TreeSet<OWLClassExpression>(ce.getOperands());
		for (OWLClassExpression exp : treeSet) {
			exp.accept(this);
		}
	}

	public boolean processNodeStreamRelation(OWLObjectPropertyExpression p_property, OWLClassExpression p_ce,
			int p_min, int p_max) {
		int min = p_min;
		int max = p_max;
		boolean found = false;

		if (min < 0 || max < 0) {
			for (int i = 0; i < this.cardinalityContainers.size(); ++i) {
				CardinalityContainer cc = this.cardinalityContainers.get(i);
				if (cc.property.equals(p_property) && cc.ce.equals(p_ce)) {
					found = true;

					cc.min = Math.max(cc.min, p_min);
					cc.max = Math.max(cc.max, p_max);

					min = cc.min;
					max = cc.max;

					if (min >= 0 || max >= 0)
						this.cardinalityContainers.remove(i);

					break;
				}
			}
		}

		if (false == found) {
			CardinalityContainer cc = new CardinalityContainer();
			cc.property = p_property;
			cc.ce = p_ce;
			cc.min = p_min;
			cc.max = p_max;
			this.cardinalityContainers.add(cc);
		}

		if (min < 0 || max < 0)
			return false;

		String pattern = null;

		if (p_property.equals(hasInput)) {
			switch (this.currentType) {
			case COMPUTATION_NODE:
			case SOURCE_NODE:
			case IRO_NODE:
			case MAP_NODE:
				pattern = "input($system,$element,$scope,$representation,$relatedEntity," + min + "," + max + ").\n";
				// + this.elementString;
				break;
			default:
				// TODO
			}
		} else if (p_property.equals(hasOutput)) {
			switch (this.currentType) {
			case COMPUTATION_NODE:
			case SOURCE_NODE:
			case IRO_NODE:
			case MAP_NODE:
				pattern = "output($system,$element,$scope,$representation).\n";
				break;
			default:
				// TODO
			}
		} else if (p_property.equals(hasOutputMap)) {
			switch (this.currentType) {
			case COMPUTATION_NODE:
			case SOURCE_NODE:
			case IRO_NODE:
				pattern = "outputMap($system,$element,$scope,$representation).\n";
				break;
			case MAP_NODE:
				pattern = "outputMap($system,$element,$type,$scope,$representation,$relatedEntity).\n";
				break;
			default:
				// TODO
			}
		} else if (p_property.equals(hasRelatedInput)) {
			switch (this.currentType) {
			case COMPUTATION_NODE:
			case SOURCE_NODE:
			case IRO_NODE:
			case MAP_NODE:
				pattern = "input2($system,$element,$scope,$representation," + min + "," + max + ").\n";
				// + this.elementString;
				break;
			default:
				// TODO
			}
		}

		if (pattern != null)
			this.printNodeStreamRelation(p_ce, pattern);

		return true;
	}

	public void printNodeStreamRelation(OWLClassExpression p_ce, String p_pattern) {
		this.currentRepresentation = null;
		this.currentScope = null;

		if (p_ce.isAnonymous()) {
			p_ce.accept(this);
		} else {
			OWLClass c = p_ce.asOWLClass();

			if (this.isSubClassOf(c, this.namedStream) || this.isSubClassOf(c, this.namedMap)) {
				for (OWLOntology ont : this.ontologies) {
					for (OWLSubClassOfAxiom ax : ont.getSubClassAxiomsForSubClass(c)) {
						ax.getSuperClass().accept(this);
					}
				}
			}
		}

		sb.append(this.replace(p_pattern));
	}

	private String replace(String p_string) {
		p_string = p_string.replace("$system", this.iRIShortName(this.currentSystem.getIRI()));
		p_string = p_string.replace("$element", this.iRIShortName(this.grounding.getIRI()));

		if (this.currentEntity != null)
			p_string = p_string.replace("$entity", this.iRIShortName(this.currentEntity.getIRI()));
		else
			p_string = p_string.replace("$entity", "any");
		if (this.currentEntityType != null)
			p_string = p_string.replace("$type", this.iRIShortName(this.currentEntityType.getIRI()));
		if (this.currentScope != null)
			p_string = p_string.replace("$scope", this.iRIShortName(this.currentScope.getIRI()));
		if (this.currentRepresentation != null)
			p_string = p_string.replace("$representation", this.iRIShortName(this.currentRepresentation.getIRI()));
		if (this.currentRelatedEntity != null)
			p_string = p_string.replace("$relatedEntity", this.iRIShortName(this.currentRelatedEntity.getIRI()));
		else
			p_string = p_string.replace("$relatedEntity", "none");

		if (p_string.contains("$information")) {
			if (this.currentEntity == null || this.currentScope == null || this.currentRepresentation == null) {
				this.log(String.format(
						"Missing information description elements '%s' (entity), '%s' (scope) '%s' (representation)",
						this.currentEntity, this.currentScope, this.currentRepresentation));
			} else {
				String info = "information("
						+ this.iRIShortName(this.currentEntity.getIRI())
						+ ","
						+ this.iRIShortName(this.currentScope.getIRI())
						+ ","
						+ this.iRIShortName(this.currentRepresentation.getIRI())
						+ ","
						+ (this.currentRelatedEntity != null ? this.iRIShortName(this.currentRelatedEntity.getIRI())
								: "none") + ")";

				p_string = p_string.replace("$information", info);
			}
		}

		return p_string;
	}

	// Generic Stuff, which I ignore

	@Override
	public void visit(OWLObjectProperty property) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDeclarationAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDatatypeDefinitionAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLAnnotationAssertionAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLSubAnnotationPropertyOfAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLAnnotationPropertyDomainAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLAnnotationPropertyRangeAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLSubClassOfAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLNegativeObjectPropertyAssertionAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLAsymmetricObjectPropertyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLReflexiveObjectPropertyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDisjointClassesAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataPropertyDomainAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectPropertyDomainAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLEquivalentObjectPropertiesAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLNegativeDataPropertyAssertionAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDifferentIndividualsAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDisjointDataPropertiesAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDisjointObjectPropertiesAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectPropertyRangeAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectPropertyAssertionAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLFunctionalObjectPropertyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLSubObjectPropertyOfAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDisjointUnionAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLSymmetricObjectPropertyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataPropertyRangeAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLFunctionalDataPropertyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLEquivalentDataPropertiesAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLClassAssertionAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLEquivalentClassesAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataPropertyAssertionAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLTransitiveObjectPropertyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLIrreflexiveObjectPropertyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLSubDataPropertyOfAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLInverseFunctionalObjectPropertyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLSameIndividualAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLSubPropertyChainOfAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLInverseObjectPropertiesAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLHasKeyAxiom axiom) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLRule rule) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectUnionOf ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectComplementOf ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectAllValuesFrom ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectHasValue ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectHasSelf ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectOneOf ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataSomeValuesFrom ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataAllValuesFrom ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataHasValue ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataMinCardinality ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataExactCardinality ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataMaxCardinality ce) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLLiteral node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLFacetRestriction node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDatatype node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataOneOf node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataComplementOf node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataIntersectionOf node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataUnionOf node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDatatypeRestriction node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLObjectInverseOf property) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLDataProperty property) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLAnnotationProperty property) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLAnonymousIndividual individual) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(IRI iri) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLAnnotation node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLClassAtom node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLDataRangeAtom node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLObjectPropertyAtom node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLDataPropertyAtom node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLBuiltInAtom node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLVariable node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLIndividualArgument node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLLiteralArgument node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLSameIndividualAtom node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(SWRLDifferentIndividualsAtom node) {
		// TODO Auto-generated method stub

	}

	@Override
	public void visit(OWLOntology ontology) {
		// TODO Auto-generated method stub

	}

	private class CardinalityContainer {
		int min = -1;
		int max = -1;
		OWLClassExpression ce;
		OWLObjectPropertyExpression property;
	}
}
