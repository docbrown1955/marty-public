<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.1">
  <compound kind="file">
    <name>abreviation.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>abreviation_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="algo_8h" name="algo.h" local="yes" imported="no">algo.h</includes>
    <includes id="index_8h" name="index.h" local="yes" imported="no">index.h</includes>
    <includes id="interface_8h" name="interface.h" local="yes" imported="no">interface.h</includes>
    <includes id="space_8h" name="space.h" local="yes" imported="no">space.h</includes>
    <includes id="utils_8h" name="utils.h" local="yes" imported="no">utils.h</includes>
    <class kind="class">csl::Abbrev</class>
    <class kind="struct">csl::Abbrev::compareParents</class>
    <class kind="class">csl::Abbreviation</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>abstract.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>abstract_8h.html</filename>
    <includes id="allocator_8h" name="allocator.h" local="yes" imported="no">allocator.h</includes>
    <includes id="enum_8h" name="enum.h" local="yes" imported="no">enum.h</includes>
    <includes id="error_8h" name="error.h" local="yes" imported="no">error.h</includes>
    <includes id="evaluation_8h" name="evaluation.h" local="yes" imported="no">evaluation.h</includes>
    <includes id="std__shared__ptr__inheritance_8h" name="std_shared_ptr_inheritance.h" local="yes" imported="no">std_shared_ptr_inheritance.h</includes>
    <includes id="support_8h" name="support.h" local="yes" imported="no">support.h</includes>
    <includes id="typedef_8h" name="typedef.h" local="yes" imported="no">typedef.h</includes>
    <class kind="class">csl::Abstract</class>
    <class kind="class">csl::Expr</class>
    <namespace>csl</namespace>
    <member kind="function">
      <type>Expr</type>
      <name>minus_</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aa1186b8efe45b2ea34ffcf8e0de69238</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>prod_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae8a7809aa1495de26d449b25c5676215</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, bool explicitProd=0)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>fraction_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>adb8fb620d09d80258c0c3d0c2a72340a</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>pow_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a39a4d5d0b5daeb571db05ae3bb2d36e4</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>derivative_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a34118703c2961443732bb747369f30ac</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, int order=1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator+</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a4214d96bf84e933c268cf30c85ef5065</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator-</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8c85cf9cc27a2f9ea69c28fed0243a66</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator*</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a51da3cbce74c766bb1aff63488113698</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator/</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a75e84d94b9caf4662aa30a5c6aa36a63</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac52343b89158d08dc27aa4851ba1662c</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a73fc26b671ec2e0fa84ba12983ed9866</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a64388fa8d8a566d1b1b565f888acfaa5</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a612503ed45700a00e76eccfd796bfec1</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abdcf5895808d59f7113b9c9dc28133ca</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6dbe8e8ea5d3a9880c2d95741728f8d1</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator|=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a187485362620c294cc6cf77f797d07a7</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&amp;=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae5f3a8cb78aea12f4d966effd040681b</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>algo.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>algo_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <namespace>csl</namespace>
    <member kind="function">
      <type>Expr</type>
      <name>FindLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae25492ddb894bb646ca12868bbd5e54d</anchor>
      <arglist>(Expr const &amp;init, Expr const &amp;value, size_t depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>FindNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a16030249dd9a145214dad11c6c698c75</anchor>
      <arglist>(Expr const &amp;init, Expr const &amp;node, size_t depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>FindIfLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a27562270e1babd293920469862804ddf</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, size_t depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>FindIfNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af08b277cdb6f9d098af7640035563e5d</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, size_t depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>AnyOfLeafs</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a0c725982c603dc7243b94238c7399f17</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>AllOfLeafs</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab34e46d9db1bf2c52a11e16edb96aac9</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>AnyOfNodes</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aee4fb7f5bf59e525f167e3e5e1bea6ed</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>AllOfNodes</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a2a09c25d95caf5531f593f5e8a92cc99</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>VisitEachNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab32d8a121c108b533a4e746a10d737ba</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; void(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>VisitEachLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a795b923fd042a73265a04f5fa551362b</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; void(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>VisitEachNodeCut</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abfb7fdd6c18bd8a98bfa3674e4bebefb</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ForEachNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8bbe21d8979e1d4fa2e9242a34c397be</anchor>
      <arglist>(Expr &amp;init, std::function&lt; void(Expr &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ForEachLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af74bfbc9faa2de7d49f5d37403795b73</anchor>
      <arglist>(Expr &amp;init, std::function&lt; void(Expr &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ForEachNodeCut</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af1844d0ef7e9f50ccd490096c4f1ff21</anchor>
      <arglist>(Expr &amp;init, std::function&lt; bool(Expr &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>FirstOfNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abe86be4e2295c35823a965faf12bb0c7</anchor>
      <arglist>(Expr &amp;init, std::function&lt; bool(Expr &amp;)&gt; const &amp;f)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>FirstOfLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af9f72de45510b266cb259a69d11cbf14</anchor>
      <arglist>(Expr &amp;init, std::function&lt; bool(Expr &amp;)&gt; const &amp;f)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>Transform</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aba0af68b54eefa2d567faed146678ad7</anchor>
      <arglist>(Expr &amp;init, std::function&lt; bool(Expr &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>alloc_monitor.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>alloc__monitor_8h.html</filename>
    <class kind="class">__data_alloc</class>
    <class kind="class">__alloc_monitor</class>
  </compound>
  <compound kind="file">
    <name>allocator.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>allocator_8h.html</filename>
    <includes id="memory__pool_8h" name="memory_pool.h" local="yes" imported="no">memory_pool.h</includes>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>buildingBlock.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>buildingBlock_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <class kind="class">csl::AbstractBuildingBlock</class>
    <class kind="class">csl::Complexified</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>cast.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>cast_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="error_8h" name="error.h" local="yes" imported="no">error.h</includes>
    <includes id="utils_8h" name="utils.h" local="yes" imported="no">utils.h</includes>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>commutation.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>commutation_8h.html</filename>
    <includes id="scalarFunc_8h" name="scalarFunc.h" local="yes" imported="no">scalarFunc.h</includes>
    <class kind="class">csl::Commutator</class>
    <namespace>csl</namespace>
    <member kind="function">
      <type>Expr</type>
      <name>Commutation</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a15620d82e51a1ba2c082a0b17ed773d0</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, int sign=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>WeakCommutation</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab82c5a52247dd5ef84518d51deade500</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, int sign=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>commutator_</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>afa314ac9077b2d18a81fa954327ab7e0</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, int t_sign=-1)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>comparison.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>comparison_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <includes id="numerical_8h" name="numerical.h" local="yes" imported="no">numerical.h</includes>
    <class kind="class">csl::Arbitrary</class>
    <class kind="class">csl::Comparator</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>conditional.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>conditional_8h.html</filename>
    <class kind="class">csl::OptionalCondition</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>counter.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>counter_8h.html</filename>
    <class kind="class">counter_iterator</class>
    <class kind="class">counter</class>
  </compound>
  <compound kind="file">
    <name>csl.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>csl_8h.html</filename>
    <includes id="abreviation_8h" name="abreviation.h" local="yes" imported="no">abreviation.h</includes>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="cslcomplex_8h" name="cslcomplex.h" local="yes" imported="no">cslcomplex.h</includes>
    <includes id="diagonalization_8h" name="diagonalization.h" local="yes" imported="no">diagonalization.h</includes>
    <includes id="evaluation_8h" name="evaluation.h" local="yes" imported="no">evaluation.h</includes>
    <includes id="functional_8h" name="functional.h" local="yes" imported="no">functional.h</includes>
    <includes id="indicial_8h" name="indicial.h" local="yes" imported="no">indicial.h</includes>
    <includes id="indicialExpander_8h" name="indicialExpander.h" local="yes" imported="no">indicialExpander.h</includes>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <includes id="lock_8h" name="lock.h" local="yes" imported="no">lock.h</includes>
    <includes id="mathFunctions_8h" name="mathFunctions.h" local="yes" imported="no">mathFunctions.h</includes>
    <includes id="numerical_8h" name="numerical.h" local="yes" imported="no">numerical.h</includes>
    <includes id="objectSymmetry_8h" name="objectSymmetry.h" local="yes" imported="no">objectSymmetry.h</includes>
    <includes id="operations_8h" name="operations.h" local="yes" imported="no">operations.h</includes>
    <includes id="operator_8h" name="operator.h" local="yes" imported="no">operator.h</includes>
    <includes id="options_8h" name="options.h" local="yes" imported="no">options.h</includes>
    <includes id="parent_8h" name="parent.h" local="yes" imported="no">parent.h</includes>
    <includes id="sort_8h" name="sort.h" local="yes" imported="no">sort.h</includes>
    <includes id="tensorField_8h" name="tensorField.h" local="yes" imported="no">tensorField.h</includes>
    <includes id="vector_8h" name="vector.h" local="yes" imported="no">vector.h</includes>
    <includes id="algo_8h" name="algo.h" local="yes" imported="no">algo.h</includes>
    <includes id="cast_8h" name="cast.h" local="yes" imported="no">cast.h</includes>
    <includes id="commutation_8h" name="commutation.h" local="yes" imported="no">commutation.h</includes>
    <includes id="comparison_8h" name="comparison.h" local="yes" imported="no">comparison.h</includes>
    <includes id="conditional_8h" name="conditional.h" local="yes" imported="no">conditional.h</includes>
    <includes id="interface_8h" name="interface.h" local="yes" imported="no">interface.h</includes>
    <includes id="interface__indicial_8h" name="interface_indicial.h" local="yes" imported="no">interface_indicial.h</includes>
    <includes id="pseudoIntegral_8h" name="pseudoIntegral.h" local="yes" imported="no">pseudoIntegral.h</includes>
    <includes id="simplification_8h" name="simplification.h" local="yes" imported="no">simplification.h</includes>
    <includes id="space_8h" name="space.h" local="yes" imported="no">space.h</includes>
    <includes id="utils_8h" name="utils.h" local="yes" imported="no">utils.h</includes>
    <includes id="counter_8h" name="counter.h" local="yes" imported="no">counter.h</includes>
    <includes id="progressBar_8h" name="progressBar.h" local="yes" imported="no">progressBar.h</includes>
    <includes id="scopedProperty_8h" name="scopedProperty.h" local="yes" imported="no">scopedProperty.h</includes>
    <includes id="symmetricCounter_8h" name="symmetricCounter.h" local="yes" imported="no">symmetricCounter.h</includes>
    <includes id="timeMonitor_8h" name="timeMonitor.h" local="yes" imported="no">timeMonitor.h</includes>
    <includes id="timer_8h" name="timer.h" local="yes" imported="no">timer.h</includes>
    <includes id="librarydependency_8h" name="librarydependency.h" local="yes" imported="no">librarydependency.h</includes>
    <includes id="libraryevaluator_8h" name="libraryevaluator.h" local="yes" imported="no">libraryevaluator.h</includes>
    <includes id="libraryfunction_8h" name="libraryfunction.h" local="yes" imported="no">libraryfunction.h</includes>
    <includes id="librarygenerator_8h" name="librarygenerator.h" local="yes" imported="no">librarygenerator.h</includes>
    <includes id="librarytensor_8h" name="librarytensor.h" local="yes" imported="no">librarytensor.h</includes>
  </compound>
  <compound kind="file">
    <name>cslcomplex.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>cslcomplex_8h.html</filename>
    <includes id="operator_8h" name="operator.h" local="yes" imported="no">operator.h</includes>
    <includes id="scalarFunc_8h" name="scalarFunc.h" local="yes" imported="no">scalarFunc.h</includes>
    <class kind="class">csl::RealPart</class>
    <class kind="class">csl::ImaginaryPart</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>default_move_cpy.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>default__move__cpy_8h.html</filename>
  </compound>
  <compound kind="file">
    <name>diagonalization.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>diagonalization_8h.html</filename>
    <includes id="vector_8h" name="vector.h" local="yes" imported="no">vector.h</includes>
    <class kind="class">csl::Diagonalizer</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>element.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>element_8h.html</filename>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <includes id="numerical_8h" name="numerical.h" local="yes" imported="no">numerical.h</includes>
    <includes id="parent_8h" name="parent.h" local="yes" imported="no">parent.h</includes>
    <class kind="class">csl::AbstractElement</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>enum.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>enum_8h.html</filename>
    <namespace>csl</namespace>
    <member kind="enumeration">
      <type></type>
      <name>PrimaryType</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a70046459ec0082db8cb97005c13c9a54</anchor>
      <arglist></arglist>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a3c1fc46575f79b15fc8990642387d07f">Arbitrary</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a5225763efc97aebd39e5a303fceb57ac">Numerical</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54ad7da1b76e5799f53a399b7a96ba67437">Literal</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a78216444cbb3388cc8c2f64cc44c7cd5">MultiFunction</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a8870e734b769add08821a829f6951853">ScalarFunction</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a97a447e7de95e9e2940291e17bcc4e3a">Vectorial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a25e27f9899952c474587a6bee3ab9978">Indicial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a6f16a5f8ff5d75ab84c018adacdfcbb7">Field</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ace591865c69b6315c533a1a325777312</anchor>
      <arglist></arglist>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a892b1f4fc1fcdaaf7dfed08ced04a389">InheritanceType</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a9080fd7a5f0af44b585bb21b62d8b283">NoType</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312aa0faef0851b4294c06f2b94bb1cb2044">Integer</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a22ae0e2b89e5e3d477f988cc36d3272b">Float</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312aa9e4acf6d1d381d6875f480a5737b8b4">IntFraction</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312aa79046cb7e9f609ba07c4ec22f14a862">IntFactorial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a10b4eb76294b70d7fd6df997ff06edb1">Complex</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ab87c58a6946efb3fd56cc53586d2fb78">NumericalEval</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af19f497d4c860d252cc1e055d0362ccc">Imaginary</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312acb17869fe51048b5a5c4c6106551a255">Constant</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a47c14840d8e15331fa420b9b2f757cd9">Variable</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312acad39a154bffb61175f674d6eefaf6d0">Exp</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ace0be71e33226e4c1db2bcea5959f16b">Log</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a29524358ad4c2579d7b08a21c990ce5e">DiracDelta</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a1e34af023adeb7d5f484f8eb4b9826b6">Abs</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312affca562be079b9e4e41ea9d6a86c582b">Cos</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a0986d137604183312e6d3599578bc6cd">Sin</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a1266b4e6f81e60733ec6c717e0181f60">Tan</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a4371c3565b771b40c187487cc0190392">Cosh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a98ed804abf4edd1f3be3fa894fa8cc3d">Sinh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312acc132a41cab5676334f353a22a0aa5c5">Tanh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a1c3ca8e525c0e6a47cc8489cb3155365">ACos</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a118d7e2878ba8789c8f86eee41c470b3">ASin</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af9be9640e98f6072e8e0eaf2b418efe2">ATan</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a27cebb75cb5855484f7df7070b4cab73">ACosh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a6964cd88a53b2e4c358bcad0d9feac55">ASinh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a2286f4e717c6213c895da49a51aa5de7">ATanh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af60357a8d17e45793298323f1b372a74">Scalar</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ac74cc04b1519df2caf5026e94c3e8bab">RealPart</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ac49d71b3e882559a62d9d17009728907">ImaginaryPart</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a84f9e7bc59dd338537142e95bbd5521a">BooleanOperator</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a2ed2c32254f39662881b6eed72021250">TensorElement</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a02e48a623b58c0b63bd2ad0e891c944e">ScalarField</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a5a44d898396f28619971f227d438ef40">TensorFieldElement</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a9134ead1dfb798c5efccfaf181501eeb">TDerivativeElement</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af575ff69fb3dc1eeb3341d4a419cada2">StandardDuo</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a607547b66dcd8e516cde5cc78739ae80">StandardMult</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a8be6d3e955290ee50c00a4bc92aa7749">Pow</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a362a33c23b08e4a32a4ec53fbb82cccd">Prod</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312aa0ec87054b5e5b7847d0d8780a01a3d5">Sum</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a24ecfbe376a82f09ad48bffc6b8d6a87">Polynomial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a70ae6e285cc14c8486e3cf5bec39d1fd">Derivative</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312acc1e31375d9c49eb15a3302b96acc061">Commutator</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a45f4ce6c3306644b1efe333f4f8d6929">Angle</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a012b11265b94945f7a1390a07776e625">Factorial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a4ea94552a2bec56a29592359a1b6069e">Integral</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ad4d8fc83b4874f82d0b698683c6b9e22">ScalarIntegral</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a7a5b7a2e7ef67739db0cf0cc4d3e5d60">VectorIntegral</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a57dea6f5039281b7fee517fc43bf3110">Vector</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af53df0293e169f562bc1d9a20e1d2589">Matrix</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a93ff2aa452bc6cbb7c3fe56cc7d196c5">HighDTensor</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ComplexProperty</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac54d9e79dc6697a8eacc07c754b11c0e</anchor>
      <arglist></arglist>
      <enumvalue file="namespacecsl.html" anchor="ac54d9e79dc6697a8eacc07c754b11c0ea10b4eb76294b70d7fd6df997ff06edb1">Complex</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ac54d9e79dc6697a8eacc07c754b11c0ea7f80fcc452c2f1ed2bb51b39d0864df1">Real</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ac54d9e79dc6697a8eacc07c754b11c0eaf19f497d4c860d252cc1e055d0362ccc">Imaginary</enumvalue>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad68e55a1e260433cbafab69addbb53c4</anchor>
      <arglist>(std::ostream &amp;fout, csl::Type type)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a073cd06df1b6c4e0efb497d67d87e275</anchor>
      <arglist>(std::ostream &amp;fout, csl::PrimaryType primaryType)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>error.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>error_8h.html</filename>
    <includes id="options_8h" name="options.h" local="yes" imported="no">options.h</includes>
  </compound>
  <compound kind="file">
    <name>evaluation.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>evaluation_8h.html</filename>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>functional.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>functional_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="interface_8h" name="interface.h" local="yes" imported="no">interface.h</includes>
    <includes id="utils_8h" name="utils.h" local="yes" imported="no">utils.h</includes>
    <class kind="class">csl::Functional</class>
    <class kind="struct">csl::Functional::Replacer</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>index.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>index_8h.html</filename>
    <includes id="std__vector__implementation_8h" name="std_vector_implementation.h" local="yes" imported="no">std_vector_implementation.h</includes>
    <class kind="class">csl::Index</class>
    <class kind="class">csl::IndexStructure</class>
    <namespace>csl</namespace>
    <namespace>cslIndex</namespace>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>ad333cdff46bc7adff9474bfb9d9d3f4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Free</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>ad333cdff46bc7adff9474bfb9d9d3f4eaf6c415d974f23a8d98634cd1e91f6ad9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Dummy</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>ad333cdff46bc7adff9474bfb9d9d3f4ea07288b108ee7f23274ef7af2fc1df9c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Fixed</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>ad333cdff46bc7adff9474bfb9d9d3f4eaea7fa93a248dbab81642463f10ba55cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>TensorType</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>aa49df84371c2616835b29cb0e5733abb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Metric</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>aa49df84371c2616835b29cb0e5733abba8f3767576db7f62a2864244c8e2841b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Delta</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>aa49df84371c2616835b29cb0e5733abba56812cd3f023e8a71d7dc6ad3904ac7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>operator!</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a7239c266a7391f8a652b83e65d927f4a</anchor>
      <arglist>(const Index &amp;index)</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>operator+</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae8258f8706933e1c10c973c721cd6151</anchor>
      <arglist>(const Index &amp;index)</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>operator-</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aa060a7f29c77b51682733901c7a24d04</anchor>
      <arglist>(const Index &amp;index)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>index_new.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>index__new_8h.html</filename>
    <includes id="error_8h" name="error.h" local="yes" imported="no">error.h</includes>
    <includes id="index_8h" name="index.h" local="yes" imported="no">index.h</includes>
    <includes id="support_8h" name="support.h" local="yes" imported="no">support.h</includes>
    <class kind="class">csl::IndexStructureView</class>
    <class kind="class">csl::IndexStructure_new</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>indicial.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>indicial_8h.html</filename>
    <includes id="element_8h" name="element.h" local="yes" imported="no">element.h</includes>
    <includes id="index_8h" name="index.h" local="yes" imported="no">index.h</includes>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <includes id="memory__pool_8h" name="memory_pool.h" local="yes" imported="no">memory_pool.h</includes>
    <includes id="numerical_8h" name="numerical.h" local="yes" imported="no">numerical.h</includes>
    <includes id="operations_8h" name="operations.h" local="yes" imported="no">operations.h</includes>
    <includes id="parent_8h" name="parent.h" local="yes" imported="no">parent.h</includes>
    <includes id="symmetry_8h" name="symmetry.h" local="yes" imported="no">symmetry.h</includes>
    <includes id="vector_8h" name="vector.h" local="yes" imported="no">vector.h</includes>
    <class kind="class">csl::SelfContraction</class>
    <class kind="class">csl::ContractionChain</class>
    <class kind="class">csl::TensorParent</class>
    <class kind="class">csl::EpsilonParent</class>
    <class kind="class">csl::DeltaParent</class>
    <class kind="class">csl::MetricParent</class>
    <class kind="class">csl::Tensor</class>
    <class kind="class">csl::TensorElement</class>
    <class kind="class">csl::ISum</class>
    <class kind="class">csl::IProd</class>
    <namespace>csl</namespace>
    <member kind="function">
      <type>void</type>
      <name>nameTensor</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab17fb7de179f8643818e2eb85c4adda8</anchor>
      <arglist>(const std::string &amp;name, Expr &amp;tensor, bool first=true)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>generateTensor</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6343f864651a51563696008ea8c73f64</anchor>
      <arglist>(const std::string &amp;name, const std::vector&lt; const Space * &gt; &amp;spaces)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>indicialExpander.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>indicialExpander_8h.html</filename>
    <includes id="counter_8h" name="counter.h" local="yes" imported="no">counter.h</includes>
  </compound>
  <compound kind="file">
    <name>interface.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>interface_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="options_8h" name="options.h" local="yes" imported="no">options.h</includes>
    <includes id="parent_8h" name="parent.h" local="yes" imported="no">parent.h</includes>
    <namespace>csl</namespace>
    <member kind="function">
      <type>void</type>
      <name>ApplySelfProperty</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>acf713387a07a13348155f7043b55c933</anchor>
      <arglist>(Expr &amp;init, csl::Tensor &amp;tensor, Expr const &amp;A, Expr const &amp;B, Expr const &amp;res)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ApplyChainProperty</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a36d313d19ed9c1b6e4cfbefbb7af7169</anchor>
      <arglist>(Expr &amp;init, csl::Tensor &amp;tensor, std::vector&lt; Expr &gt; const &amp;prod, Expr const &amp;res)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>CheckValidity</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a9b1baa69edc83c2d90efd6f79bbdb0b4</anchor>
      <arglist>(Expr const &amp;init, std::vector&lt; Expr_info &gt; encountered=std::vector&lt; Expr_info &gt;())</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>MemorySizeOf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a769c039db45134ce7cbdd918da92273b</anchor>
      <arglist>(Expr const &amp;expression)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>interface_indicial.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>interface__indicial_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>libcomplexop.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>libcomplexop_8h.html</filename>
  </compound>
  <compound kind="file">
    <name>libcomplexop_hdata.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>libcomplexop__hdata_8h.html</filename>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>libdiagonalization.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>libdiagonalization_8h.html</filename>
    <class kind="class">Diagonalizer</class>
  </compound>
  <compound kind="file">
    <name>libdiagonalization_cppdata.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>libdiagonalization__cppdata_8h.html</filename>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>libdiagonalization_hdata.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>libdiagonalization__hdata_8h.html</filename>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>libmakefile_data.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>libmakefile__data_8h.html</filename>
  </compound>
  <compound kind="file">
    <name>librarydependency.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>librarydependency_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <class kind="class">csl::LibDependency</class>
    <class kind="struct">csl::LibDependency::IncludeType</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>libraryevaluator.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>libraryevaluator_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="index_8h" name="index.h" local="yes" imported="no">index.h</includes>
    <class kind="class">csl::LibEval</class>
    <class kind="class">csl::LibEvalSession</class>
    <class kind="struct">csl::LibEvalSession::Perf</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>libraryfunction.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>libraryfunction_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="indicial_8h" name="indicial.h" local="yes" imported="no">indicial.h</includes>
    <includes id="default__move__cpy_8h" name="default_move_cpy.h" local="yes" imported="no">default_move_cpy.h</includes>
    <includes id="libraryevaluator_8h" name="libraryevaluator.h" local="yes" imported="no">libraryevaluator.h</includes>
    <class kind="struct">csl::LibParameter</class>
    <class kind="class">csl::LibFunction</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>librarygenerator.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>librarygenerator_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="librarydependency_8h" name="librarydependency.h" local="yes" imported="no">librarydependency.h</includes>
    <includes id="libraryfunction_8h" name="libraryfunction.h" local="yes" imported="no">libraryfunction.h</includes>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <class kind="class">csl::LibraryGenerator</class>
    <class kind="struct">csl::LibraryGenerator::file</class>
    <class kind="struct">csl::LibraryGenerator::DiagonalizationData</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>librarytensor.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>librarytensor_8h.html</filename>
  </compound>
  <compound kind="file">
    <name>librarytensor_hdata.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>librarytensor__hdata_8h.html</filename>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>literal.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>literal_8h.html</filename>
    <includes id="buildingBlock_8h" name="buildingBlock.h" local="yes" imported="no">buildingBlock.h</includes>
    <includes id="variableParent_8h" name="variableParent.h" local="yes" imported="no">variableParent.h</includes>
    <class kind="class">csl::AbstractLiteral</class>
    <class kind="class">csl::Constant</class>
    <class kind="class">csl::Variable</class>
    <class kind="class">csl::IntFactorial</class>
    <class kind="class">csl::Imaginary</class>
    <namespace>csl</namespace>
    <member kind="function">
      <type>Expr</type>
      <name>intfactorial_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a7fbc14af6671615ab035d1c56fdcc02a</anchor>
      <arglist>(int value)</arglist>
    </member>
    <member kind="variable">
      <type>const csl::Expr</type>
      <name>CSL_INF</name>
      <anchorfile>literal_8h.html</anchorfile>
      <anchor>ab8e84e9b107229a13eec6315f4b1f9e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const csl::Expr</type>
      <name>CSL_UNDEF</name>
      <anchorfile>literal_8h.html</anchorfile>
      <anchor>ac71928d4f12a2b15fe4157b9dced95a2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>lock.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>lock_8h.html</filename>
    <class kind="class">csl::Lock</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>mathFunctions.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>mathFunctions_8h.html</filename>
    <includes id="scalarFunc_8h" name="scalarFunc.h" local="yes" imported="no">scalarFunc.h</includes>
    <class kind="class">csl::Abs</class>
    <class kind="class">csl::Exp</class>
    <class kind="class">csl::Log</class>
    <class kind="class">csl::Cos</class>
    <class kind="class">csl::Sin</class>
    <class kind="class">csl::Tan</class>
    <class kind="class">csl::ACos</class>
    <class kind="class">csl::ASin</class>
    <class kind="class">csl::ATan</class>
    <class kind="class">csl::Angle</class>
    <class kind="class">csl::Cosh</class>
    <class kind="class">csl::Sinh</class>
    <class kind="class">csl::Tanh</class>
    <class kind="class">csl::ACosh</class>
    <class kind="class">csl::ASinh</class>
    <class kind="class">csl::ATanh</class>
    <class kind="class">csl::Factorial</class>
    <class kind="class">csl::DiracDelta</class>
    <namespace>csl</namespace>
    <member kind="function">
      <type>Expr</type>
      <name>abs_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a5327d368e944125c5ffa58b304051751</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>exp_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a677874110c8d7d3febc7f30085273e6b</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>log_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a77a31d3871e8136ab04e15368528c846</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>cos_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a3033bff78373155994cbf45ec3e71493</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>sin_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a5f030b62fc81f11147493796bfa30b54</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>tan_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>afbaab772a8fb873c65e06d079e73cd06</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>acos_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a51f0625e0729c8516296eaa4e82a9928</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>asin_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac5fe6fa63f0b6cd80fd9edd625a715ad</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>atan_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a55a28054aa1541d982b05e4c90e22243</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>cosh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a9f96bc23faf2e85278c67d2e77b02430</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>sinh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a2aab7af831ac14498b23ac3daa80d1d5</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>tanh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8a0975103bcda47f0e7a335677ba8e1f</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>acosh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a832f3758b9ec7e3f52e554b949c8acf3</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>asinh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6f8d17a63615e912a201b11ce163b9dd</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>atanh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac7d65507d0aa73726a4f0969ba1134be</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>memory_pool.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>memory__pool_8h.html</filename>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>numerical.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>numerical_8h.html</filename>
    <includes id="buildingBlock_8h" name="buildingBlock.h" local="yes" imported="no">buildingBlock.h</includes>
    <class kind="class">csl::AbstractNumerical</class>
    <class kind="class">csl::Integer</class>
    <class kind="class">csl::Float</class>
    <class kind="class">csl::IntFraction</class>
    <class kind="class">csl::Complex</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>objectSymmetry.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>objectSymmetry_8h.html</filename>
    <includes id="std__vector__implementation_8h" name="std_vector_implementation.h" local="yes" imported="no">std_vector_implementation.h</includes>
    <class kind="class">csl::ObjectPermutation</class>
    <class kind="class">csl::ObjectSymmetry</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>operations.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>operations_8h.html</filename>
    <includes id="operator_8h" name="operator.h" local="yes" imported="no">operator.h</includes>
    <includes id="scalarFunc_8h" name="scalarFunc.h" local="yes" imported="no">scalarFunc.h</includes>
    <class kind="class">csl::Sum</class>
    <class kind="class">csl::Polynomial</class>
    <class kind="class">csl::Prod</class>
    <class kind="class">csl::Pow</class>
    <class kind="class">csl::Derivative</class>
    <class kind="class">csl::Integral</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>operator.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>operator_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="commutation_8h" name="commutation.h" local="yes" imported="no">commutation.h</includes>
    <includes id="interface_8h" name="interface.h" local="yes" imported="no">interface.h</includes>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <includes id="numerical_8h" name="numerical.h" local="yes" imported="no">numerical.h</includes>
    <includes id="utils_8h" name="utils.h" local="yes" imported="no">utils.h</includes>
    <class kind="class">csl::Operator</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>options.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>options_8h.html</filename>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>parent.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>parent_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <class kind="class">csl::AbstractParent</class>
    <class kind="class">csl::Parent</class>
    <namespace>csl</namespace>
    <namespace>cslParent</namespace>
    <member kind="enumeration">
      <type></type>
      <name>PrimaryType</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a83f078a60370b86ccd6b113eedff51c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Indicial</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a83f078a60370b86ccd6b113eedff51c2ac11c4f92f20580eb0d84f14ec49e41ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Field</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a83f078a60370b86ccd6b113eedff51c2a578739351cbc014d9a0b31850b6899a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>GenericIndicial</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339aa84cb71d316a2d84edc0717cb4ca0efc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Metric</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339ad30e0f678eff5f93c8bb23ff181de4d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Delta</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339aa6b88b45ff88168338ab570f6c640d53</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ScalarField</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339acaa17b0c5fa8433f006fb1823931dc17</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TensorFieldElement</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339ab3dc2eea537b69b0d85acc45dbc548a1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>progressBar.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>progressBar_8h.html</filename>
    <includes id="timeMonitor_8h" name="timeMonitor.h" local="yes" imported="no">timeMonitor.h</includes>
    <class kind="class">csl::ProgressBar</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>pseudoIntegral.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>pseudoIntegral_8h.html</filename>
    <includes id="operations_8h" name="operations.h" local="yes" imported="no">operations.h</includes>
    <includes id="mathFunctions_8h" name="mathFunctions.h" local="yes" imported="no">mathFunctions.h</includes>
    <includes id="indicial_8h" name="indicial.h" local="yes" imported="no">indicial.h</includes>
    <class kind="class">csl::PseudoIntegral</class>
    <class kind="class">csl::AbstractIntegral</class>
    <class kind="class">csl::ScalarIntegral</class>
    <class kind="class">csl::VectorIntegral</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>scalarFunc.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>scalarFunc_8h.html</filename>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <includes id="numerical_8h" name="numerical.h" local="yes" imported="no">numerical.h</includes>
    <includes id="std__vector__implementation_8h" name="std_vector_implementation.h" local="yes" imported="no">std_vector_implementation.h</includes>
    <class kind="class">csl::AbstractFunc</class>
    <class kind="class">csl::AbstractDuoFunc</class>
    <class kind="class">csl::AbstractMultiFunc</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>scopedProperty.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>scopedProperty_8h.html</filename>
    <class kind="class">csl::ScopedProperty</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>simplification.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>simplification_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <namespace>csl</namespace>
    <member kind="function">
      <type>void</type>
      <name>printVector</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a4dc45c94957fc5b977345f3710daa971</anchor>
      <arglist>(const csl::vector_expr &amp;vector)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addAlternateForm</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a75d23a79a366d8af5519ca40795d1d98</anchor>
      <arglist>(csl::vector_expr &amp;alternateForms, const Expr &amp;newAlternate, bool add_factor_expand=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reduceAlternate</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abbdbabfc34997aff95b6a271afa8de2f</anchor>
      <arglist>(csl::vector_expr &amp;alternateForms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearRedundancyAlternate</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac7fe69183e8550e7fce8019a409bde81</anchor>
      <arglist>(csl::vector_expr &amp;alternateForms)</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr</type>
      <name>getRecursiveAlternateForms</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad3617f2bee57ef5bf099950c54160a4e</anchor>
      <arglist>(const Expr &amp;expr, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr</type>
      <name>internalRecursiveAlternateForms</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8756d45335e213a1323b7ee840134e18</anchor>
      <arglist>(const Expr &amp;expr, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Simplify</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad27c8d3dd60c235e3e804b97790110ab</anchor>
      <arglist>(const Expr &amp;expr, int depth=-1)</arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>SIMPLIFICATION_METHOD</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a87993eddca471f11b91e56d071675420</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>MAX_ALTERNATE_FORMS</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a4cfdfd259da7ff4d06994eed7d4b99ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>MAX_RECURSION_ALTERNATE</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>adbdaaf75e2d3ed39cc0e33633d2300dc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>sort.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>sort_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <namespace>csl</namespace>
    <member kind="function">
      <type>void</type>
      <name>sort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a30af37876f8c6102a9a0ffcb83c78f13</anchor>
      <arglist>(std::vector&lt; Expr &gt; &amp;argument)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a038131f4648d3a85d7f66956b9a052f9</anchor>
      <arglist>(std::vector&lt; Expr &gt;::iterator first, std::vector&lt; Expr &gt;::iterator last)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>selectionSort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>adcde11ad745cfee42e9efa83d145031a</anchor>
      <arglist>(std::vector&lt; Expr &gt; &amp;argument)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>selectionSort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aaa60374eb375314f6f7c661a35c01e67</anchor>
      <arglist>(std::vector&lt; Expr &gt;::iterator first, std::vector&lt; Expr &gt;::iterator last)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mergeSort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a1c48c85e78f6b663a82bd7e100ad68d0</anchor>
      <arglist>(std::vector&lt; Expr &gt; &amp;argument)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mergeSort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af4d5f0bc5267d581906fe5a40f3dcce1</anchor>
      <arglist>(std::vector&lt; Expr &gt;::iterator first, std::vector&lt; Expr &gt;::iterator last)</arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>minMergeSize</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab3885c426f3cbbc274fe4ce33aae739b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>space.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>space_8h.html</filename>
    <includes id="indicial_8h" name="indicial.h" local="yes" imported="no">indicial.h</includes>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <includes id="numerical_8h" name="numerical.h" local="yes" imported="no">numerical.h</includes>
    <includes id="vector_8h" name="vector.h" local="yes" imported="no">vector.h</includes>
    <class kind="class">csl::Space</class>
    <namespace>csl</namespace>
    <member kind="function">
      <type>const Space</type>
      <name>Euclid_R2</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>adc609bb5307e2707f433fd90847da159</anchor>
      <arglist>(&quot;R2&quot;, 2)</arglist>
    </member>
    <member kind="function">
      <type>const Space</type>
      <name>Euclid_R3</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae2032bf9b522dae416eb0c7f66642e89</anchor>
      <arglist>(&quot;R3&quot;, 3)</arglist>
    </member>
    <member kind="function">
      <type>const Space</type>
      <name>Euclid_R4</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8d1d8eb1cfa212c44e0ef199e78db957</anchor>
      <arglist>(&quot;R4&quot;, 4)</arglist>
    </member>
    <member kind="variable">
      <type>const Space &amp;</type>
      <name>Minkowski</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aef525ce7226ae5fa0fc9b9f2aa4ccc5d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>std_shared_ptr_inheritance.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>std__shared__ptr__inheritance_8h.html</filename>
  </compound>
  <compound kind="file">
    <name>std_vector_implementation.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>std__vector__implementation_8h.html</filename>
  </compound>
  <compound kind="file">
    <name>support.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>support_8h.html</filename>
    <namespace>csl</namespace>
    <member kind="function">
      <type>int</type>
      <name>PGCD</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8c9bbef2158b7cb97a7d45e8739e01d5</anchor>
      <arglist>(double a, double b)</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>PGCD</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a293f777248d9b27ff737fef8d51bb544</anchor>
      <arglist>(long long int a, long long int b)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>internal_PGCD</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a776ff8b234198c45db2d6233508ad558</anchor>
      <arglist>(long long int a, long long int b)</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>sgn</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a62ae14b0bc1a6b3f7e9138421b2b8360</anchor>
      <arglist>(long long int a)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>sgn</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a36cdda72e72136f4b0afec1d865361c5</anchor>
      <arglist>(double a)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>factorial</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a5bd20a50cf5cdfbe57e3b896b980179e</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>internal_factorial</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a88fe2378efd87d7bc05ba13dbb35fa9e</anchor>
      <arglist>(long long int n)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>comparePlaceIndependant</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a31f687274fba7d8f4dc865a21a1901ac</anchor>
      <arglist>(std::vector&lt; T &gt; A, std::vector&lt; T &gt; B)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>partialComparePlaceIndependant</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad90b89dc5d8450e4ce9538431f80925b</anchor>
      <arglist>(std::vector&lt; T &gt; A, std::vector&lt; T &gt; B)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>supportParent.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>supportParent_8h.html</filename>
  </compound>
  <compound kind="file">
    <name>symmetricCounter.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>symmetricCounter_8h.html</filename>
    <includes id="std__vector__implementation_8h" name="std_vector_implementation.h" local="yes" imported="no">std_vector_implementation.h</includes>
    <class kind="class">symmetricCounter</class>
  </compound>
  <compound kind="file">
    <name>symmetry.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>symmetry_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="index_8h" name="index.h" local="yes" imported="no">index.h</includes>
    <includes id="std__vector__implementation_8h" name="std_vector_implementation.h" local="yes" imported="no">std_vector_implementation.h</includes>
    <class kind="class">csl::Permutation</class>
    <class kind="class">csl::Symmetry</class>
    <class kind="class">csl::IndexedSymmetry</class>
    <namespace>csl</namespace>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; int &gt; &gt;</type>
      <name>permutations</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad16a083cc7c50de69d67b66c20e030f2</anchor>
      <arglist>(std::vector&lt; int &gt; init)</arglist>
    </member>
    <member kind="function">
      <type>vector&lt; Permutation &gt;</type>
      <name>permutations</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a35c72686b5438eec934374e4d151e4fc</anchor>
      <arglist>(const Permutation &amp;init)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reducePermutation</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad292d5ba73b91dee12de72e4c96996b3</anchor>
      <arglist>(std::vector&lt; Permutation &gt; &amp;permutation)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Permutation &gt;</type>
      <name>getSpan</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6fe98d1ad16fa3c08f8a35317438fcb1</anchor>
      <arglist>(const std::vector&lt; Permutation &gt; &amp;init)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>getSpan</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a67a9246a8bb37b33f7117f1e1eb571f9</anchor>
      <arglist>(std::vector&lt; Permutation &gt; &amp;spanned, const Permutation &amp;element)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>tensorField.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>tensorField_8h.html</filename>
    <includes id="parent_8h" name="parent.h" local="yes" imported="no">parent.h</includes>
    <includes id="indicial_8h" name="indicial.h" local="yes" imported="no">indicial.h</includes>
    <class kind="class">csl::TensorFieldParent</class>
    <class kind="class">csl::TensorField</class>
    <class kind="class">csl::TensorFieldElement</class>
    <class kind="class">csl::TDerivativeElement</class>
    <class kind="class">csl::TDerivativeParent</class>
    <class kind="class">csl::TDerivative</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>timeMonitor.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>timeMonitor_8h.html</filename>
    <class kind="class">csl::TimeMonitor</class>
    <class kind="struct">csl::TimeMonitor::Time</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>timer.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>timer_8h.html</filename>
    <class kind="class">csl::Timer</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>typedef.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>typedef_8h.html</filename>
    <includes id="allocator_8h" name="allocator.h" local="yes" imported="no">allocator.h</includes>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>utils.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>utils_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <namespace>csl</namespace>
    <member kind="function">
      <type>Expr</type>
      <name>Copy</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab7420e1ff6f6a7d9f069bca1c6ac16f5</anchor>
      <arglist>(const Abstract *expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Copy</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab074eed4bd06ee44010c91e2d1dd05cb</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>DeepCopy</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a40f0cf1575083e98534941e8aee6cfad</anchor>
      <arglist>(const Abstract *expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>DeepCopy</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af1e310aad90377d99f591def8b30336c</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Refreshed</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a9c5e3c2f5f4c6533ac74d150d29f931d</anchor>
      <arglist>(const Abstract *expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Refreshed</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a90a30db275050c669fbe24152402c28e</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>DeepRefreshed</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae95bf5940e1dda061d2d9fbdd1a9fff8</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>variableParent.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>variableParent_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="parent_8h" name="parent.h" local="yes" imported="no">parent.h</includes>
    <class kind="class">csl::LiteralParent</class>
    <class kind="class">csl::ConstantParent</class>
    <class kind="class">csl::VariableParent</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="file">
    <name>vector.h</name>
    <path>/home/gregoire/marty-public/src/csl/</path>
    <filename>vector_8h.html</filename>
    <includes id="abstract_8h" name="abstract.h" local="yes" imported="no">abstract.h</includes>
    <includes id="literal_8h" name="literal.h" local="yes" imported="no">literal.h</includes>
    <includes id="numerical_8h" name="numerical.h" local="yes" imported="no">numerical.h</includes>
    <class kind="class">csl::AbstractVectorial</class>
    <class kind="class">csl::Vector</class>
    <class kind="class">csl::Matrix</class>
    <class kind="class">csl::HighDTensor</class>
    <namespace>csl</namespace>
  </compound>
  <compound kind="class">
    <name>__alloc_monitor</name>
    <filename>class____alloc__monitor.html</filename>
  </compound>
  <compound kind="class">
    <name>__data_alloc</name>
    <filename>class____data__alloc.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Abbrev</name>
    <filename>classcsl_1_1Abbrev.html</filename>
    <class kind="struct">csl::Abbrev::compareParents</class>
  </compound>
  <compound kind="class">
    <name>csl::Abbreviation</name>
    <filename>classcsl_1_1Abbreviation.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>csl::Expr</type>
      <name>getExactEncapsulated</name>
      <anchorfile>classcsl_1_1Abbreviation.html</anchorfile>
      <anchor>a9503bc71a0694eddede5c14f0ac767fb</anchor>
      <arglist>(Expr_info self) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>getEncapsulated</name>
      <anchorfile>classcsl_1_1Abbreviation.html</anchorfile>
      <anchor>a4e8386b97a78005096ba14af29a2213e</anchor>
      <arglist>() const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Abs</name>
    <filename>classcsl_1_1Abs.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Abs</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>ab0569268981dbf81400dbee0852616b3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Abs</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>a8884fd283b0e572e2386b8dde280aa6c</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>aec650f01292bf5d1381d587e489a4810</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>a08ced320bad3c444c1d51efde39c068b</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>a4dcecd9b1e1dbca2386e4af8dd663ba5</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>ac08757293e89b6352824ae09843a5c18</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>abd250986b9ce084c98fb968db0fb587b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>afe47e46685e38164123937fffa8c3072</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Abs.html</anchorfile>
      <anchor>a3b735634cb0181f048b6fd23dea5a614</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Abstract</name>
    <filename>classcsl_1_1Abstract.html</filename>
    <member kind="function">
      <type></type>
      <name>Abstract</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ad5fee68c1f3dcbda0e936926fed17616</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Abstract</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a40bdc4c76fdbbf3cbac3af4dce8286ed</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aed56cb85894209a9bbc9b8bdada5e6ab</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const =0</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>printExplicit</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a89e45ffc7b65f92e77e592f76f70934c</anchor>
      <arglist>(int mode=0) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa067d4a97135bfe2052a7e21c28f75be</anchor>
      <arglist>(int mode=0) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::string const  &amp;</type>
      <name>getName</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>af7f5e7466e754984a2c4394a112464f7</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a2263731ca94825f35281e69fb258cf87</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a8ab79682cdff4c7d7b52c2e70ac23d85</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa781456c3f801b37ce169c83514c51c1</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getDim</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a5856c3d55a6d1406970f3656e61efe8e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isBuildingBlock</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ad74c068f1da78856321932b6b6ae17dc</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a216b7bd3930fbf047e5bf8f69e9b964c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isInteger</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a9a014bfd266b5f8471f23580e271979b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>getValued</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a628563238145f3a9acc331f1a89ce32f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual long double</type>
      <name>getValue</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a78f98ba06476e464e3fd2fce0d1f8243</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual long long int</type>
      <name>getNum</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a6aa6bcc7cb8ee1574f29dc6ab6c6f0ed</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual long long int</type>
      <name>getDenom</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a9531d78d67c82e288c0130da5b1ba80b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getNArgs</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa034448201c9e7998bbbb5e409636b30</anchor>
      <arglist>(int axis=0) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual csl::vector_expr::iterator</type>
      <name>begin</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aad54ed41b2ce9bad3364fe30ad1a0d71</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual csl::vector_expr::iterator</type>
      <name>end</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a61e4e80d998a888a1a74d9ea3e10c0f1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual csl::vector_expr::const_iterator</type>
      <name>begin</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>adff373df9477d4e38581d09cc6d2fee4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual csl::vector_expr::const_iterator</type>
      <name>end</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ae2761d54f13310e13f5076d565edd7ef</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr const  &amp;</type>
      <name>getArgument</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ac33a42309d629d47bfdbede4e152414e</anchor>
      <arglist>(int iArg=0) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr const  &amp;</type>
      <name>getArgument</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ad94b3892af147ac0b05c1d8c960e4eba</anchor>
      <arglist>(const std::vector&lt; int &gt; &amp;indices) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual const csl::vector_expr &amp;</type>
      <name>getVectorArgument</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ac83c44ae1e5315a38083bdb7acd8f3fa</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getVariable</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ae85f810bbadbc792210c958e8f1b2ffb</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getOrder</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a9500a21d75d03c695ec0c76493600b38</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getSign</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa83df1d9dc202457113e5b38bd694fae</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isAnOperator</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a9347e75b0cd0b75e0f0a38db93b29d83</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isEmpty</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a464f4f0613db9b4b0a4efe26a3a03d60</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getOperand</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a9c3ec3dfe1fbd8ead5c8d9aca421a863</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::vector&lt; int &gt;</type>
      <name>getShape</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a625efe55a17b97a1c0f085bdbf3a962a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getNIndices</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>add364193f7a78ac28f7001c7f4036453</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Index</type>
      <name>getIndex</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a3234ce243f6a2e11950be42f10100943</anchor>
      <arglist>(int i=0) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ae125a32b6f3d0368b6055d4e58c1ff3f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual IndexStructure</type>
      <name>getFreeIndexStructure</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa4136dc23f1fba4bf897b54c8592ccfc</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Parent</type>
      <name>getParent</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ab150130c9f67daaf6aba52b9f6b90941</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getNContractedPairs</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a42125e9765a771c47819548d342b0b28</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual csl::vector_expr</type>
      <name>getPermutations</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a946d175d946da102c5f4cc91840703cb</anchor>
      <arglist>(bool optimize=true) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::set&lt; std::pair&lt; int, int &gt; &gt;</type>
      <name>getContractedPair</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>af04c0e4426f0bf9df365afe0148d59df</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setName</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a867135c96776248add60018f4aa7a578</anchor>
      <arglist>(const std::string &amp;t_name)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setCommutable</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a7bb29d316b8f64652cef472a6c23f001</anchor>
      <arglist>(bool t_commutable)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setValue</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aaf7f0692cb84ca861a5b3b641bea518b</anchor>
      <arglist>(long double t_value)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aab2d2ac186b56d17d2cc0a105e145b11</anchor>
      <arglist>(const Expr &amp;expr, int iArg=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ae7907d6983f47d261748870b497f6aa8</anchor>
      <arglist>(const Expr &amp;expr, const std::vector&lt; int &gt; &amp;indices)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setOperand</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a70df7be4486affa8dc891e3f25bf499f</anchor>
      <arglist>(const Expr &amp;operand)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setOperandPrivate</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ae7b8a14fc746ed72e4d5abd63373c5ba</anchor>
      <arglist>(const Expr &amp;operand, bool leaveEmpty)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setVectorArgument</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a83fee8ed28079bcac107455d58be9d5d</anchor>
      <arglist>(const csl::vector_expr &amp;t_argument)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>insert</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a5437e7f685edf8549e6d3b34572343f8</anchor>
      <arglist>(const Expr &amp;expr, bool side=1)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>replaceIndex</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>acc0eb077e25c9a67d15269cf8d5f0511</anchor>
      <arglist>(const Index &amp;indexToReplace, const Index &amp;newIndex, bool refresh=true) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setIndexStructure</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a70f946f9c59df045ef7416320ade89c7</anchor>
      <arglist>(const IndexStructure &amp;t_index)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setFullySymmetric</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a4f90ebb7cf7c26782bd36bc2a67fc5d4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setFullyAntiSymmetric</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ac318016372d14b795893fe7e59199084</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>addSymmetry</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ad431770faa4291116a50175193a11a88</anchor>
      <arglist>(int i1, int i2)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>addAntiSymmetry</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a5549ac0c186b3c6febf332245a25a82c</anchor>
      <arglist>(int i1, int i2)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>permut</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>abe9aceabbc0a99642e84c9025dbdc767</anchor>
      <arglist>(int i1, int i2)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getNumericalFactor</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aad25c117c8dfc4d9718ba41e8d3f33a6</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getNFactor</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a204dbd37b63432cb08932b995f49d3f4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual csl::vector_expr</type>
      <name>getFactors</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ac34c9c6f05e692faeed2f030514787ad</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>getTerm</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a1d36e7a5acc50766d5cd3f6d59df978d</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>getExponents</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ac513c930caa9939c2b9557f36a34ee69</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, std::vector&lt; Expr &gt; &amp;exponents) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>checkIndexStructure</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa0c316b059cb2a1e3020310b148f921b</anchor>
      <arglist>(const std::vector&lt; Index &gt; &amp;t_index) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>checkIndexStructure</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a1ebd84082bb93418408cc0babe2dffe2</anchor>
      <arglist>(const std::initializer_list&lt; Index &gt; &amp;index) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a7caee12abb451168149779a668dfc59e</anchor>
      <arglist>(Expr_info expr, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aea8459b3206eb3a2297eeea7688e9520</anchor>
      <arglist>(Expr_info t_variable) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>askTerm</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a27ad666bf7d533607da9675b0c645ee9</anchor>
      <arglist>(Expr_info expr, bool exact=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a40fe2ccf376dadd21b2d7a0f134d86c6</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ac3c9887977249e4d71ea36c6377b3581</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>commutesWith</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a3f68d0cd93dfe2ac02d6d4afc80fb41a</anchor>
      <arglist>(Expr_info expr, int sign=-1) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>findSubExpression</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a299b1fd6c2b9d80ad90a08c7acef28d2</anchor>
      <arglist>(Expr_info subExpression, const Expr &amp;newExpression) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>isPolynomial</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>acae10d5685ccec295fbd9be7996073cb</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>matchShape</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ab79c1d8df9e6b1a3eca66958f7dc489d</anchor>
      <arglist>(Expr_info expr, bool exact=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>hasContractionProperty</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa31e08c251d96b8ea8ec8d385895c1ff</anchor>
      <arglist>(Expr_info B) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>contraction</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ad874fc858295a737e6ce884530f1109b</anchor>
      <arglist>(Expr_info B) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a543d5a4f0432e166b492ad28a860a65f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa2d5a98ec433dad08613083c12f587cd</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const =0</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aecab8c872023a2d0771e7e4f83bb05f8</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a0bc0de9c4ce5f77fc716af0f502d13d3</anchor>
      <arglist>(bool full=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a8fb3d657c65606d520688db6401fe650</anchor>
      <arglist>(Expr_info factor, bool full=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>collect</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ab3746f1d3667846824ea51b8a2e00d31</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, bool full=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>suppressTerm</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a43520e1d6d5fdad9b50f77c6e027f928</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>suppressExponent</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a4eeedaeaa39e75facf1060dce7c57a8f</anchor>
      <arglist>(Expr const &amp;factor, Expr const &amp;exponent) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a3a87ab082152fea4fc2444659028b840</anchor>
      <arglist>(bool full=false, bool inPlace=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aeb48aaaa856582b6359037fe94d2419b</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inPlace=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a415f64fdddaae445014d5e052956f889</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a43f44bbb3685fc9f4a0402223b2e1004</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a859f792aec52ba1fe1181f81d1cc3451</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a9cc64c66894acafc1f069dc40072084b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a03e8bdba20c6192a948892f212f09dbe</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>getPolynomialTerm</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ade540a127da7304f2b7282f4dcaffc7f</anchor>
      <arglist>(Expr_info t_variable, int order) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>applyOperator</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a316956fea77627a716500a2bbfc1f3b7</anchor>
      <arglist>(const Expr &amp;operand, bool leaveEmpty=false) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>addition_own</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a101219973341a39cf15f07e5e1aa388f</anchor>
      <arglist>(const Expr &amp;expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>multiplication_own</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a4f5ed458ba62f5b4cf61a9e1f8568af1</anchor>
      <arglist>(const Expr &amp;expr, bool side=1) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>division_own</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a1578c74aee1050b806a67e7a81a67f5c</anchor>
      <arglist>(const Expr &amp;expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>exponentiation_own</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa2696a0feb43766ef54e1e3dd3d6d134</anchor>
      <arglist>(const Expr &amp;expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getRegularExpression</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a5b77f008f4b95030641224877004f06c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>tensordot</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a7b83c6a13716688f45b02e6291d39aba</anchor>
      <arglist>(const Expr &amp;expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>dot</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a4cc7a46deaa3eaa7593909d74e76d4af</anchor>
      <arglist>(const Expr &amp;expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getSum</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aeaf8154e39ff211b12e6382e675ccaad</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getProduct</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a07489d0569bc983718796d21c73f34b3</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getVectorialModulus</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a0188b049736016c970a2513ed9f6931f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getSubVectorial</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a7dcdc11b64c8c515933882168d0792ef</anchor>
      <arglist>(int iExcept) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getSubVectorial</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a413c50b2663f57d38c1d3ec3cfeaa3ce</anchor>
      <arglist>(int iExcept, int jExcept) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getSubVectorial</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a0f88bae88e30b37cc3a4d4c7a476a992</anchor>
      <arglist>(const std::vector&lt; int &gt; &amp;exceptions) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>determinant</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aeb08811cf370616477a92075e4a218ee</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>trace</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a9a86d21f5c5f65eb7e524a8f8002c0e7</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>trace</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aacab43e551e9259a39a79378373ce49c</anchor>
      <arglist>(int axis1, int axis2) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>transpose</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a1e97712696e5c40b28bc1e35987fea94</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>symmetrise</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a4faf1ffd8cb3b6c721e128980c0303d8</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>antisymmetrise</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>acd972aed8326d4c13534594e061ee6da</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>inverseMatrix</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a1b68943aa7ecf60a63af0f2f368eef8f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>operator=</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a59ff2950fc8ca319f87a730450f14545</anchor>
      <arglist>(double t_value)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a8334e14cd5789168b037d3888ed60edd</anchor>
      <arglist>(int t_value) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a12d929e34039e817e464abc560bd4a9d</anchor>
      <arglist>(double t_value) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>af5b62a6308638064c42da80e9cc2cb7e</anchor>
      <arglist>(int t_value) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>af4fd1b57df371dac7ccfd5a909620e49</anchor>
      <arglist>(double t_value) const</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a3775f3088e784cc924cb7d26e118660e</anchor>
      <arglist>(Expr_info expr) const =0</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>ac99a619abde7e4c58f39e14d4d20fcf5</anchor>
      <arglist>(const Expr_c &amp;expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr const  &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>aa8331053065d1be2da89e70c45d24863</anchor>
      <arglist>(int iArg) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a560aa9993d506551c54a8c02d1445814</anchor>
      <arglist>(int iArg)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>operator&lt;</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a5c1ba8dce7fbc19eb98807bd15a279c9</anchor>
      <arglist>(Expr_info expr) const =0</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a6e19587260a07dba65cf2a985e05516b</anchor>
      <arglist>(const Expr_c &amp;expr) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a485c6f3e10db539a70416cd0766b117d</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>a4c8883df19d7538b68fa96aadac9dfa9</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>classcsl_1_1Abstract.html</anchorfile>
      <anchor>afbcac6df2fd7ac11f336189cf215124b</anchor>
      <arglist>(const Expr_c &amp;expr) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractBuildingBlock</name>
    <filename>classcsl_1_1AbstractBuildingBlock.html</filename>
    <base>csl::Abstract</base>
    <member kind="function">
      <type>bool</type>
      <name>isBuildingBlock</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>aee083330a4490359012cf0e656758a44</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>commutesWith</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>add4dd8d4ed4ea0d8817fdd79abd744cc</anchor>
      <arglist>(Expr_info expr, int sign=-1) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>a7cbbd2edd75a84057f1622826c1efd3a</anchor>
      <arglist>(bool full=false, bool inplace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>a2f12cede626ece5382c9869d32c32146</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>a447f940bf0a08d13297fde0b50b4cb84</anchor>
      <arglist>(Expr_info expr, bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getTerm</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>a58c627e31c2a42adec4caf39030123aa</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>acd32aa346383a68ef69a48b56669deff</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>a09e06ecdb4eb9c6f8434c19ede046fa1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getPolynomialTerm</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>a05d8a8cf5ba9ae9563c09b65bac10c2f</anchor>
      <arglist>(Expr_info expr, int order) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>findSubExpression</name>
      <anchorfile>classcsl_1_1AbstractBuildingBlock.html</anchorfile>
      <anchor>a4000d3f204c1957f5cffc0677dd754c0</anchor>
      <arglist>(Expr_info subExpression, const Expr &amp;newExpression) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractDuoFunc</name>
    <filename>classcsl_1_1AbstractDuoFunc.html</filename>
    <base>csl::Abstract</base>
    <member kind="function">
      <type></type>
      <name>AbstractDuoFunc</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a1c4814f068da0301ed0f0088739f3c99</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a96910b875bda50dd7111e0f03477e36d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>abafb2415e1952440db4b16c6138d6313</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a032adea087db90b6ed2c222942fdebd6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a3c528c48d0b95db2e506f1d6959c5e67</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a7b6c806179b294bbbcd606cb9257fd99</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNArgs</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a2effb7bc0153b8c4e6d3c695d3b245cb</anchor>
      <arglist>(int axis=0) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>getArgument</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a6db2c19ea96bee60189534d583fc2af3</anchor>
      <arglist>(int iArg=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>findSubExpression</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a86ac8ca4a6eed274d004d989f3a65f3d</anchor>
      <arglist>(Expr_info subExpression, const Expr &amp;newExpression) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>ab9ed03e7424e97a17464fb76e14bb213</anchor>
      <arglist>(const Expr &amp;t_argument, int iArg=0) override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>abccb4ba9f313edd3a3939c317afbc14e</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a484a42c8d029d02f2def2df2bf24c7c0</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>ab8d3db6601adde22f9cfcaa5b046bcbe</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a14a9ddd387b869c129c48dfaea5974ad</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a04866fe8e757f391e79d6aaa18885221</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>replaceIndex</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>aeaa65642df3b0311a2f0056335011ab9</anchor>
      <arglist>(const Index &amp;indexToReplace, const Index &amp;newIndex, bool refresh=true) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a0a9110ce4150f135902865efdd99c130</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a8bfd43c575fadc19e46948953af5f2a5</anchor>
      <arglist>(Expr_info expr, bool full=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>collect</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>ac4f47febdbe3b773bce05b39d506e452</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, bool full=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a5dbc9bf69fbe0beb1034b21f83ba6aba</anchor>
      <arglist>(bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>adda5dcff671af0afed2ce13d81b1a903</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a423dcc1eccb833f88b3f98caa52189a9</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a6642c1edceccda7928bbd70256446428</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>commutesWith</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>ac84821d717ca9ab1aed295b644fe8cae</anchor>
      <arglist>(Expr_info expr, int sign=-1) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>isPolynomial</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a424f5c3e08f3548d1be123b599eaac2f</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>aaa1bbff7462d592af2ad699c02296107</anchor>
      <arglist>(int i) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a485adbc87cb3b4739eb468cfd09e7b31</anchor>
      <arglist>(int i) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>a2d20154a943a0a723791d0a6223382fa</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::array&lt; Expr, 2 &gt;</type>
      <name>argument</name>
      <anchorfile>classcsl_1_1AbstractDuoFunc.html</anchorfile>
      <anchor>aa3958b228b6f0ba7b0761abf42a4608e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractElement</name>
    <filename>classcsl_1_1AbstractElement.html</filename>
    <base>csl::Complexified</base>
    <member kind="function">
      <type></type>
      <name>AbstractElement</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>a12ebb2cd662c44ca2e60106d630dbc48</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>AbstractElement</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>a90b37b4752b24ab6cb0a29dc3329eea4</anchor>
      <arglist>(const std::shared_ptr&lt; AbstractParent &gt; &amp;t_parent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~AbstractElement</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>a96ee7c198c82450d17ce0f08df2bef00</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Parent</type>
      <name>getParent</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>a9e50d48ea0ee472c5c07f3985cc6c641</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::string const  &amp;</type>
      <name>getName</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>ae80699e259632a43c4f69a125b64ae30</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>ab3418bd6d3548316885c8bda0a7464b8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParent</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>af55db25120693bf64ed360105cb4357c</anchor>
      <arglist>(const Parent &amp;t_parent) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setName</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>aed5a0c9faec5b41fb1feb52764c4237f</anchor>
      <arglist>(const std::string &amp;t_name) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCommutable</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>ab8f5f04326a3320b48a315ec5cee60d9</anchor>
      <arglist>(bool t_commutable) override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>isPolynomial</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>a9f3e30eeaace6c5c21cda6895daa2de9</anchor>
      <arglist>(Expr_info variable) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getPolynomialTerm</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>a028ecff66278c1e385280b761388b929</anchor>
      <arglist>(Expr_info variable, int order) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>ad0583927014b475eb57853429c91df20</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::shared_ptr&lt; AbstractParent &gt;</type>
      <name>parent</name>
      <anchorfile>classcsl_1_1AbstractElement.html</anchorfile>
      <anchor>a5ad0a45c8a069be5e3e0ce21b784bb0b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractFunc</name>
    <filename>classcsl_1_1AbstractFunc.html</filename>
    <base>csl::Abstract</base>
    <member kind="function">
      <type></type>
      <name>AbstractFunc</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a5fdb13c5df0c5d7201378c183acb31fa</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a18df1a5dbd7c9c75b1704fe958a55a5d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>ab41faa67ed807f3cc2c95f5f175e9099</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a25e37d9d40927ef216a5b10958fa83ba</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>af16a648041a09ff87602427add048ee0</anchor>
      <arglist>(Expr_info expr, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const override</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a53ef9c900304f02f54ce6ea1485b4537</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNArgs</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>ae32e5909a9d7616ceac05b22a74b1284</anchor>
      <arglist>(int axis=0) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>getArgument</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>adf8cb8650883f5ea7729cdabec2aeab0</anchor>
      <arglist>(int iArg=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>ab1d0e4f2b4d063838158829972c2160a</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a24aee4ffda2255a56fd2eb2e04c9baad</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a83aa54cdff8a16f0a305358d992096f1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>ae67e2d46dfb78e3278fc2134b7d039e4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a3a466c8804d5c855dd2b906be2b1be5f</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>findSubExpression</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a2844b245ea8608c32be1ae2794a12858</anchor>
      <arglist>(Expr_info subExpression, const Expr &amp;newExpression) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a701d14b482a47a85c6cd5266118604cb</anchor>
      <arglist>(const Expr &amp;t_argument, int iArg=0) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>replaceIndex</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a0714debb1e4fe6d8b6bf1b915771b30c</anchor>
      <arglist>(const Index &amp;indexToReplace, const Index &amp;newIndex, bool refresh=true) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>ad13204b56adf098d9c4b886ea7b8681f</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a281d2d19b84b9b5fec66dbf1eb89a27d</anchor>
      <arglist>(Expr_info expr, bool full=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>collect</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a2b1a66c3b926cd7a95060464a3bb6a62</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a8bc591288c134387b71c758229d65f54</anchor>
      <arglist>(bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a3be99a2a139aeb34596b598bafa9975d</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a8c3e75947048293c9c6cc0c8e261049f</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>aa67b2133ef52393513ecbdc72e45acb0</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>commutesWith</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>ae2422de442a3c2b7d3011012a5cb7a46</anchor>
      <arglist>(Expr_info expr, int sign=-1) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>isPolynomial</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a699c366fc3c292e8a70941e1c4ee5b0c</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a224c6e6a26a42ac2dba35e30e2e7a521</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>af3afc58a715c978290e4860101fb3c27</anchor>
      <arglist>(int i) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>ab6a82539b4c61521f13b39bf1b230e68</anchor>
      <arglist>(int i) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a7ccf43edff01ef8c6500b5117ac42213</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Expr</type>
      <name>argument</name>
      <anchorfile>classcsl_1_1AbstractFunc.html</anchorfile>
      <anchor>a9ffcf8afbee0a103421280b82cfffe05</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractIntegral</name>
    <filename>classcsl_1_1AbstractIntegral.html</filename>
    <base>Operator&lt; AbstractFunc &gt;</base>
    <member kind="function">
      <type></type>
      <name>~AbstractIntegral</name>
      <anchorfile>classcsl_1_1AbstractIntegral.html</anchorfile>
      <anchor>aa3e51512953288ae5f34dff91d067129</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getOperand</name>
      <anchorfile>classcsl_1_1AbstractIntegral.html</anchorfile>
      <anchor>a0e22ef62f91cf61f7d3067d7ae4af007</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOperand</name>
      <anchorfile>classcsl_1_1AbstractIntegral.html</anchorfile>
      <anchor>a65250576bfa5547fe8eb17c37e0d3ed7</anchor>
      <arglist>(const Expr &amp;t_operand) override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>suppressTerm</name>
      <anchorfile>classcsl_1_1AbstractIntegral.html</anchorfile>
      <anchor>ac5fa23e3bd78501937629e10d1938ea5</anchor>
      <arglist>(Expr_info term) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1AbstractIntegral.html</anchorfile>
      <anchor>a03f67aec1f7e9b74fd65383e0fc6600f</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1AbstractIntegral.html</anchorfile>
      <anchor>a9570853b5def96e519e54fc81a14f840</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1AbstractIntegral.html</anchorfile>
      <anchor>a72eadc4f6452f7139be88c9bfe0c8649</anchor>
      <arglist>(Expr_info var) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1AbstractIntegral.html</anchorfile>
      <anchor>ab675c7672e9523a8fdfaf71f9741bb93</anchor>
      <arglist>(Expr_info var) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractLiteral</name>
    <filename>classcsl_1_1AbstractLiteral.html</filename>
    <base>csl::Complexified</base>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1AbstractLiteral.html</anchorfile>
      <anchor>aa41c4506afb800db5eca9232df00720a</anchor>
      <arglist>() const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractMultiFunc</name>
    <filename>classcsl_1_1AbstractMultiFunc.html</filename>
    <base>csl::Abstract</base>
    <member kind="function">
      <type></type>
      <name>AbstractMultiFunc</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>ad7c6d28772b2684c20a42bbfcb8cf257</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a77d642a8a42845ad66da5faa78cb3c27</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a1b6044133a95c411324d20afe460c55b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a2bc449bd37728479b8c6b0a9eea3bccf</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>aee96fe96e6cf30e1022ee8f489884aa5</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNArgs</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>af12bbdfb8a1b45d722d37f2444443457</anchor>
      <arglist>(int axis=0) const override</arglist>
    </member>
    <member kind="function">
      <type>const csl::vector_expr &amp;</type>
      <name>getVectorArgument</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a5e910beb3bdf8459306fe0cda78c1c76</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>getArgument</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a75ae783c1bd8e55ccc005af538599def</anchor>
      <arglist>(int iArg=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>findSubExpression</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>acb17ac601dd6fb6c98a6de6983d41009</anchor>
      <arglist>(Expr_info subExpression, const Expr &amp;newExpression) const override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr::iterator</type>
      <name>begin</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a6236c27a797993973fb45dc9f4906962</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr::iterator</type>
      <name>end</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a178ccaec7d4546873084591d93398153</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr::const_iterator</type>
      <name>begin</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>aa405a3e5cd03bb390ce3e804fe8af806</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr::const_iterator</type>
      <name>end</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a42f8f99108068dca2bd7a259577a0bcf</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a6cf9bbbcd0b0b3f75121f1c0a5cf4d57</anchor>
      <arglist>(const Expr &amp;t_argument, int iArg=0) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setVectorArgument</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a8e6df749f2e0afdbe8ae0993235dda54</anchor>
      <arglist>(const csl::vector_expr &amp;t_argument) override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a4740c75c01e1d8aeb7db075fc5377789</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a232e9782e303c525a18f436e5e5f97b9</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>ac1bea898b7169caabc409badff44c710</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a46c3300eb0f87234002c476d3c7c9a14</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>abd84cb1aa54ef9875bae6f806ed1d753</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>replaceIndex</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a636313fd58465a56d3b41cb74f9ffaff</anchor>
      <arglist>(const Index &amp;indexToReplace, const Index &amp;newIndex, bool refresh=true) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>ad9eed372c276bc55ce8614a94962fef6</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a71ffe88901a200f59bdb88d4adf09a44</anchor>
      <arglist>(Expr_info expr, bool full=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>collect</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>accd66bc16179cf88684a6db68e277921</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, bool full=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>aa7b240760c1c7d930053fbd38c812196</anchor>
      <arglist>(bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>af09641d4d997d869626ff660ca4d3f8b</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a9d64fefa1c3718d1fe431edfa4934e71</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>ad072ccb3f8007430a6d6dce53f32b441</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>commutesWith</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>ac54eff0c503e2c2947b232622261c998</anchor>
      <arglist>(Expr_info expr, int sign=-1) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>isPolynomial</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a8014e107c584b6450d3cc1ce78e154ca</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>ad4be44fbde5ebd97065a575992d498c2</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a26979a899fab4621102645d0f8366457</anchor>
      <arglist>(int i) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>ae4922013ae4c2b23e6fab8bc9056ab42</anchor>
      <arglist>(int i) override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>csl::vector_expr</type>
      <name>argument</name>
      <anchorfile>classcsl_1_1AbstractMultiFunc.html</anchorfile>
      <anchor>a8e44404eb3e6c23f7c7ddf10cdd3a152</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractNumerical</name>
    <filename>classcsl_1_1AbstractNumerical.html</filename>
    <base>csl::AbstractBuildingBlock</base>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1AbstractNumerical.html</anchorfile>
      <anchor>ab546a5a971f3547b7bbf2940465d70e9</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getNumericalFactor</name>
      <anchorfile>classcsl_1_1AbstractNumerical.html</anchorfile>
      <anchor>ac3c2bbe76a12e7e7d08009b82e1fb7c1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getTerm</name>
      <anchorfile>classcsl_1_1AbstractNumerical.html</anchorfile>
      <anchor>a092aae72532fd13bcc79392bd18ebfad</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1AbstractNumerical.html</anchorfile>
      <anchor>af911961cd47a4cec8c31cb9fafaf74d6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1AbstractNumerical.html</anchorfile>
      <anchor>afe459bd13460da89d77acb73d19baa59</anchor>
      <arglist>(Expr_info) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1AbstractNumerical.html</anchorfile>
      <anchor>a1cc3b916f6ff6b0b63c17095c1375adc</anchor>
      <arglist>(Expr_info) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractParent</name>
    <filename>classcsl_1_1AbstractParent.html</filename>
    <member kind="function">
      <type></type>
      <name>AbstractParent</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a3a8495f4cc8281410f8aa512c64116cb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>AbstractParent</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a961d277c9a277163931cb5b45906c83f</anchor>
      <arglist>(const std::string &amp;t_name)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>AbstractParent</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>ad88d1dd79f22b67744640124252dc093</anchor>
      <arglist>(const AbstractParent &amp;abstract)=default</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~AbstractParent</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a6d13d559a8d68562b5827073b31b3c1a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string const  &amp;</type>
      <name>getName</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>ab22f321502a8aa481865be3e58bb241d</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>af8ae5696856c551204addb95271333bf</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setName</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a13e6f74e590cbf05dcafb4d9c3df8b9c</anchor>
      <arglist>(std::string t_name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCommutable</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>aae1bc83d1df8237e6bf50b6b8f732716</anchor>
      <arglist>(bool t_commutable)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual cslParent::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a02948077d67665cbe33ba0f8febda859</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual cslParent::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a6a9115038be74e35dbcd8ecdbba7e5a1</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getDim</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>ac45297ef35dab1807a158056ec8dfac3</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>getDim</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a6c81260a7b64e93b88dea1935b524505</anchor>
      <arglist>(const Space *t_space) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>getFullySymmetric</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>af56df7eb3d90348eda50e3d6cc5cd8b0</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>getFullyAntiSymmetric</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>ab04c96ff80ee2f0786e59a7eafd71175</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::vector&lt; Permutation &gt;</type>
      <name>getPermutation</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>ad42c4b140d48c9319e0ae47436d040e1</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getTensor</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>adcc94486a83de9c73de39f78b1eafea4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>getTrace</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a4f86be4ff457fb18becedcca645b33f4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>adf55413672b9d58875485edd1db4e563</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a74258391e7eae5a8cd0d629eabfe9ca9</anchor>
      <arglist>(Expr_info expr) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>addSpace</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>ac691c3943a7bd3eb9c671927e2f91dc7</anchor>
      <arglist>(const Space *t_space)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setFullySymmetric</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a3e866d96e2600f1f6f723bf62dee42b0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setFullyAntiSymmetric</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a8ecc485d2526a3975b79b0c4a3da21d9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>addSymmetry</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a0a4a585286e48ff062c33e96ec09531b</anchor>
      <arglist>(int i1, int i2)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>addAntiSymmetry</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a5548d7f976f21a25d657fc723f7f7132</anchor>
      <arglist>(int i1, int i2)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setSymmetry</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a79ad3927cad378f4e83055533ea20d46</anchor>
      <arglist>(const Symmetry &amp;t_symmetry)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setTensor</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a5dd9dd3730422db222a395139786ea4b</anchor>
      <arglist>(const Expr &amp;t_tensor)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>hasContractionProperty</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a7f680e00248dd59a1545488ec4b43c07</anchor>
      <arglist>(const Abstract *self, Expr_info B) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>contraction</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a4df179280a63c410c84f7ea4edbcbee2</anchor>
      <arglist>(const Abstract *self, Expr_info B) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>addSelfContraction</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a3132e0cbebcf7e578da01c188ee75e45</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, const Expr &amp;res, std::optional&lt; std::function&lt; bool(Expr_info, Expr_info)&gt;&gt; condition=std::nullopt)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>operator()</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a8a7f7e45ca51c0720c2234d22191d196</anchor>
      <arglist>(Index index)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>operator()</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>aa9298445ca3a71e7af13ff61382c8c0f</anchor>
      <arglist>(std::vector&lt; Index &gt; indices)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a2ad6b4cd58d9b051a4d62f343bf48c2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::string</type>
      <name>latexName</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a98c857ebf734c15e7afd0d8a13988ac9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>commutable</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a177e5244cc4d4b6c1e0600a4f77b523f</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classcsl_1_1AbstractParent.html</anchorfile>
      <anchor>a276319c2a0786772350b93c84f9bcbd8</anchor>
      <arglist>(std::ostream &amp;fout, const AbstractParent &amp;)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::AbstractVectorial</name>
    <filename>classcsl_1_1AbstractVectorial.html</filename>
    <base>csl::Abstract</base>
    <member kind="function">
      <type>std::string const  &amp;</type>
      <name>getName</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>aae5db88feee1be51e3ad429c6c10df58</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>afeaa37536bfa9de67a736d2884578514</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a8a568746d53548b8621fda445258e830</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>afaf73d258e30ae78513cfaa4d7c65d59</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getDim</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>afda6fa063a671a3a0feee716621c19c3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>afb6d645d2f0673a500eaf294ff822269</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNArgs</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a2689157ec6445f198e36dadb4d9aef78</anchor>
      <arglist>(int axis=0) const override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr::iterator</type>
      <name>begin</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>aa8225d3dd07be192735c766e8d267774</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr::const_iterator</type>
      <name>begin</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a135f3df162ac8e7128a3e666e26cb81b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr::iterator</type>
      <name>end</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a06f69d432a047d7b48757d8362de68d4</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr::const_iterator</type>
      <name>end</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>ada11a590d79c87baf92a98888ee32c44</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a2a2c1314e65baf839542e6fb7845ad44</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>ab99d86f5bed81171962f0cf99415bfba</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a4d3d4dbce8837fc40b19131a019044a3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a1c8b58da63fe78c40174e6f2f582cbf3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>getArgument</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a2320c9f9c71eb81a2348175dd14404a9</anchor>
      <arglist>(int iArg=0) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>getArgument</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a0b390a72cda8abec3954bfc8229cb0f7</anchor>
      <arglist>(const std::vector&lt; int &gt; &amp;indices) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>acca79f2a53255ee6b10d51efc658cb19</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a5dfd1e8233507457f725e10e2624ec0f</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>commutesWith</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a7a90b9f26666f4aff635bfbf7cd865b6</anchor>
      <arglist>(Expr_info expr, int sign=-1) const override</arglist>
    </member>
    <member kind="function">
      <type>const csl::vector_expr &amp;</type>
      <name>getVectorArgument</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>ad91b47a309a8fc9c5ed9e4b827d84155</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; int &gt;</type>
      <name>getShape</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a8cee385d5b699162c8a400e4b44ed7d7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>ae67f1daf87e45e15b82dd2827f6ba55f</anchor>
      <arglist>(const Expr &amp;expr, int iArg=0) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a47a8b2fbd9233c706cb5fcf05f96bc7f</anchor>
      <arglist>(const Expr &amp;expr, const std::vector&lt; int &gt; &amp;indices) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setVectorArgument</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a2d072812c1918ec0097729d353003198</anchor>
      <arglist>(const csl::vector_expr &amp;t_argument) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>matchShape</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>ab24d4b6eae7dbe9783aabf2bfcb7a224</anchor>
      <arglist>(Expr_info expr, bool exact=false) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getSum</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>aa9d43ea7296950633aa2ac848173cb51</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getProduct</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a6f4f1f7c516ee3daceaf5044a40c04d1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getSubVectorial</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a002b8834bc010144ab0666c6697a5d7b</anchor>
      <arglist>(const std::vector&lt; int &gt; &amp;exceptions) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getVectorialModulus</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a8f4326ec0f618785cf0e9d84b448cdd6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>dot</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a0ed41f66d207d34ebafc90bc473109ca</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>addition_own</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a91561bc3d02d055e86b7f3e35a76e25c</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>multiplication_own</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a81eebf482c8b342b75aaec063533790c</anchor>
      <arglist>(const Expr &amp;expr, bool side=1) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>tensordot</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a0e4336f06daf87990d127a4b755c121c</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>trace</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a9a79c1dcc4d4ee22c8662ffcc11e5378</anchor>
      <arglist>(int axis1, int axis2) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>symmetrise</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a690f2f1b0748bbd81213335c47535113</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>antisymmetrise</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a71a8d453b3d5c57f2d09b17e99302ef8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a09396a6b80940ac8c99604e9b845c815</anchor>
      <arglist>(bool full=false, bool inplace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a8907475ad6468725bafe0c81dfbeb688</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inplace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a9b1f0b9354e966da0d740965ec068ccd</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a368c2a6b5fb359cc8f6f209ec548f0ad</anchor>
      <arglist>(Expr_info expr, bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a45bc5fb9f24b918a685eef9742a4c046</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>afd91a1187f15759c61d5a9460f88d3dc</anchor>
      <arglist>(int iArg) override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1AbstractVectorial.html</anchorfile>
      <anchor>a303d252e82b4e28ec9df8a5a971acf52</anchor>
      <arglist>(int iArg) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ACos</name>
    <filename>classcsl_1_1ACos.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>ACos</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>a126cdbe28f59fe0fadc5dcd47e188830</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ACos</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>af5ca226ff33d462f0ef3fe68f8da3a5b</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>a6533f24c81b914bd04f440e19cbb3a16</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>a16301f8ed54cac63ac5389fc179f24d6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>aab3e1ceb5273ff6ada3c1d0d207f75d3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>a9363acfe89a647a562322a036fe68469</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>a745db1e3e6c7e9afb433a89b0f10b0a0</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>ac3a70d756d2c6f91ee35dc46f845b7f5</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>acd36159ea83e46fe2ca50a3ada149bd5</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>a24cd3961c5c39c3788a702410c437808</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>a17e119e2f6d22f0b6c1f149e2ebefcd5</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>aa0c7176fd612d3d1ed3f1f61eb900399</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1ACos.html</anchorfile>
      <anchor>a95a6076fdc3cae921b5cde90a835e74e</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ACosh</name>
    <filename>classcsl_1_1ACosh.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>ACosh</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>a7a18938379c18b661c66d3b8d2e0f8a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ACosh</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>acd0c86668bce981f596a78cda546f0f4</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>a1cd22dc7959d90a17fa747c05f1ac70a</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>a1c9ce73586a90e166932281d29a8090f</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>aca6f5306a978ba8589f2757c0599473a</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>a3cece5a5ecb328b31e7f235b43bdab67</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>a1b3ac8c059d26ae9142d9f240f7aeb78</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>acb4452efbe7c588945a6f4e78d69d2fc</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1ACosh.html</anchorfile>
      <anchor>a2dcbe0e8c6fc07d7d4ca3b02c24ced42</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Angle</name>
    <filename>classcsl_1_1Angle.html</filename>
    <base>csl::AbstractDuoFunc</base>
    <member kind="function">
      <type></type>
      <name>Angle</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>af641fb8201339c8f0322370210ada690</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Angle</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>ab624f7b3f5961b0435ee187a20aea816</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>a2a0a138a9196c8c5f195a822f7a8a982</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>a6efd87ab8730302f18e72b19431cf3b7</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>a51993a606446fead779049e3581905f8</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>ab6f5cc0b8e682f2d670d65c76402a738</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>a2f0b4708a98d477767a53218adf0affa</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>a739e0603cbf9fdee3c3a5373ff96a04a</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>a92d43b635f0e38a6c1bc57875cf7341d</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Angle.html</anchorfile>
      <anchor>a1c2c4dff4cecb505f6d9ce05701dfde0</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Arbitrary</name>
    <filename>classcsl_1_1Arbitrary.html</filename>
    <base>csl::AbstractLiteral</base>
    <member kind="function">
      <type></type>
      <name>~Arbitrary</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>a562b537606260f291b89d6b8e39741c9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string const  &amp;</type>
      <name>getName</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>a25dce08825f0efbf6715c8db35f981e6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>a489b4b7b94c0880c843db0e4cfc249d4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>ad803441e87d253727977a2731de05e3e</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>ab5f7c2c30475e2dd9a13423923ff13bc</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>a2275ec73b2842df28cb80bd59bbf958d</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>ab4744294dedcb8b4402b76f821b275cf</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>getNum</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>a0e1f9042536b8ec8f361e7fd556b58d4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Arbitrary.html</anchorfile>
      <anchor>a3bd3172760cb664c0c49475d26079d06</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ASin</name>
    <filename>classcsl_1_1ASin.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>ASin</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>af26aa02b8bf894310b9f466efa17fb0b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ASin</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>a72b9eb7d9b1c7e2dd3e9a44a3e8902c6</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>aff7945ce6a7782ffc619ebcef75ef15f</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>ac0f8218b321a2294612e25d3e57c2318</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>ab7c943492f3eb479cc7f846049e86811</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>a776b7c1794fe574114e1e3198d73fa18</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>a390673a164b5daae797188ebebc2a4fc</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>ac1da66cfd9b73db5a47e92279011dd08</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>ae9ef3ca87aa148c1bdcdb4d62ae9f2dd</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>af81b867514a85671642e52445b18e4d9</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>afd6d69a61c924d596c1f7adb859893b0</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>afec8424240e853545a33c5c260195716</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1ASin.html</anchorfile>
      <anchor>a142897732f1f6c347bc4e65c344bbb27</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ASinh</name>
    <filename>classcsl_1_1ASinh.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>ASinh</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>a0eb47846850ba500d6360829c5ef9de5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ASinh</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>a5a220630e2d3df0c2b81a5147bc930af</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>a8864eabb24d35309797271892591a61f</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>ab6ee807bccbe4394943f169ccc620c1f</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>a470c061e6a77cedcf9f57bd01a53e1dc</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>a22b8dc76ae36362de29bb2614925a8b6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>a6489126c02c25e4cfb295db5d85cadd5</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>a4fb38ca160a4e8e770885ecaf2cc62f8</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1ASinh.html</anchorfile>
      <anchor>a22a4862646c4cbaffcff45a8876c0648</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ATan</name>
    <filename>classcsl_1_1ATan.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>ATan</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>aef16996e7bd408bbc9fc9a1b21f0cccd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ATan</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>a78b0fe91adc4b515fe0f773ee2332a09</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>aef622a35036fcdd32527b881dc14e559</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>a309557f2c6332130e57396181f0cc64f</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>a851f824336d8fd176d2cc9264bfd8fcf</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>a5cc44ce49897c4488538e7b2f8ecf0b3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>a275592a7ba6a9985e921b3a791dc461f</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>a4b4ed4cfcd4c834f3a99ac2a474bad51</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1ATan.html</anchorfile>
      <anchor>abce45fbcda74eb7acf909c5c711176c0</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ATanh</name>
    <filename>classcsl_1_1ATanh.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>ATanh</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>ab5d6f7f7f4c2c19c5fec4a1ba9bab7e6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ATanh</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>a587f2dffcdc174788aff67dc58e77bc6</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>adb3c6afec3dde358d4afc272fe3e6177</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>a16e9431afda48a0838e0f036501da476</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>adb8ee7249faa4bf5c723e0f8c998ea02</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>a7a486ca2937c53a6d5940c2245da87b5</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>ad9ee39eda573051de9b45699ed886f83</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>a96c5e0b223b93c2a4469a73dfe3924f6</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1ATanh.html</anchorfile>
      <anchor>afb2c14500cc281ef0a642294bda73e99</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::BooleanOperator</name>
    <filename>classcsl_1_1BooleanOperator.html</filename>
    <base>csl::AbstractMultiFunc</base>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71df</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>True</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa4b87ab62a22ef93915aeab5d3e66c0e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>False</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa652e3712556859d9514ce19ea6925f8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>EqualTo</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa1320889116d4d65109df80b4dc6db218</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DifferentThan</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfaabad72f018ca9fe54c7382a1799a24ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LessThan</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa349e84a1b2dc0c3fb94e5ad057595a23</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LessThanOrEqualTo</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfab4e44a57136d186bef4988f06fcd5832</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>GreaterThan</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa8419a8e8a849c88dd94013088fe495c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>GreaterThanOrEqualTo</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa07ad7773c38542cfa16440de39b0e642</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>True</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa4b87ab62a22ef93915aeab5d3e66c0e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>False</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa652e3712556859d9514ce19ea6925f8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>EqualTo</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa1320889116d4d65109df80b4dc6db218</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DifferentThan</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfaabad72f018ca9fe54c7382a1799a24ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LessThan</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa349e84a1b2dc0c3fb94e5ad057595a23</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LessThanOrEqualTo</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfab4e44a57136d186bef4988f06fcd5832</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>GreaterThan</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa8419a8e8a849c88dd94013088fe495c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>GreaterThanOrEqualTo</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a12f3f98c065fe964a43a49bb636e71dfa07ad7773c38542cfa16440de39b0e642</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BooleanOperator</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>acffd1a74920041e9195c542753675698</anchor>
      <arglist>(Type type, csl::Expr const &amp;A, csl::Expr const &amp;B, csl::Expr const &amp;exprIfTrue, csl::Expr const &amp;exprIfFalse)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BooleanOperator</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a8a9d1f0e54ea04816aa2716960e51984</anchor>
      <arglist>(Type type, csl::Expr const &amp;A, csl::Expr const &amp;exprIfTrue, csl::Expr const &amp;exprIfFalse)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>add90f411fddb5ef6874ba38119c77dea</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a23ae1ecdbcef223a30e11aaa6c992732</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>aaedb35156c894b14afd91ca75d620fd3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a54853bc9447a6cf90ab9f6adc56e5825</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>ae0c3f8e40eb89cd4e5b6c9ed8ee88ae1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>af1a8f3a0f97d62f6da78d016b15415dd</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a2e61ed30827b9a42bf9b8a5ec24301ad</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a629e5fb52431fa6cd91c245b920acd47</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a7c7105988581c2948846ef14a43335b0</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1BooleanOperator.html</anchorfile>
      <anchor>a508a61cfb27b1a6ce030e21470796b32</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>csl::canDecay</name>
    <filename>structcsl_1_1canDecay.html</filename>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>csl::Commutator</name>
    <filename>classcsl_1_1Commutator.html</filename>
    <base>csl::AbstractDuoFunc</base>
    <member kind="function">
      <type></type>
      <name>Commutator</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a3f63774349dcc190336049aa125f6630</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Commutator</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a729e7e9f827945e43fc019afd02b8f0f</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, int t_sign=-1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Commutator</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a21b402d175169a4bfa0e102cc50d95ff</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>adac744ad959f7f67f5274f6725efd2b6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getSign</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>abac8376052445d0359e4919f781f185d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a4cb9ff06c9a69c826758da4b992d15b9</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a1f359f9d2379cabeec2522b6bbbd034a</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a1540b8e45df030f7828a092370feafaf</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a41beafb3d55c2f33e20a904a09ded5a9</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a74087e962f48d16fce0c02537372eea6</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>ad15cf75bcb8e00335ac9f8fd36f1b048</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>ab26b56f71268855477174401069842d1</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>sign</name>
      <anchorfile>classcsl_1_1Commutator.html</anchorfile>
      <anchor>a9506fe75fd6500339192173fd7beda57</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Comparator</name>
    <filename>classcsl_1_1Comparator.html</filename>
    <member kind="function" static="yes">
      <type>static Expr</type>
      <name>dummy</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a1767b748a3402f58de49260d129ca3fb</anchor>
      <arglist>(int n, csl::Type type=csl::Type::NoType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>dummyComparison</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a01941d1944ba76ba9e64a96790691a95</anchor>
      <arglist>(Expr_info expr, Expr_info dummyExpr)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>getFreeIndexComparisonActive</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a8463dcff2b3947a8bef04b967559de10</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::string</type>
      <name>dummyName</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a8151fd299b266af1ff1024509bfca08d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>freeIndexComparison</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>ab82c75000df87b5832f77e034d064656</anchor>
      <arglist>(const Abstract *A, Expr_info B)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>compare</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a8f2adfad8d2b38800921bacf46f149a2</anchor>
      <arglist>(const Index &amp;A, const Index &amp;B)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>clear</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a1a0c1e04a6e774f555bb0477f94378c4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>getDummyComparisonActive</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a735dcce08cfddb3c3a165457febd5b2f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>setDummyComparisonActive</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>acf17ebe8d6f379905a0fe7b6374da7c7</anchor>
      <arglist>(bool state)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static bool</type>
      <name>freeIndexComparisonActive</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a5ddaac1ebf244586939fbe6a767b7ada</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::map&lt; Index, Index &gt;</type>
      <name>indexCorrespondance</name>
      <anchorfile>classcsl_1_1Comparator.html</anchorfile>
      <anchor>a90e1a76a1117bdd19d16e536a952656b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>csl::Abbrev::compareParents</name>
    <filename>structcsl_1_1Abbrev_1_1compareParents.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Complex</name>
    <filename>classcsl_1_1Complex.html</filename>
    <base>csl::AbstractNumerical</base>
    <member kind="function">
      <type></type>
      <name>Complex</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a383d6b164ce284ae9af78cf4f9e4d876</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Complex</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a9605e33addd9cbf8959de4ee2e4d4d3a</anchor>
      <arglist>(Expr t_real)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Complex</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a0eb102abdbd9fd4d3293884c76e68e0e</anchor>
      <arglist>(Expr t_real, Expr t_imag)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>ab203dc18378296c2b7d484773bec8746</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a840991e02d47506b35ee49b16d8dd8b0</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a8405fa57577ecae2935fa40b703db69b</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a4a5a2813d3cba5cc8c761959f3dd17fb</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a0651cc75f7010f12fdd80cd0d458b568</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a0a71859b7fdc28a6cd36290823282746</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a38b17d76b63dc1afdb59c4405b1aff6d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a3cbe87d94da0eeca27369afc69f2fa4d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>afc7177213e2612bddfe1e299847a0386</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>ae87c5c2100541896a3b474d5908a6e3d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>multiplication_own</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a79889fc60628f60564ed7a60b1fad1a3</anchor>
      <arglist>(const Expr &amp;expr, bool side=1) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>addition_own</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a69a2af4ec8f4f12a56cc1fc41223bc78</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>division_own</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a63e2aab7c637beb7fa67ccbc9b8b8c12</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>exponentiation_own</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a46dbf92ea6c22239857b6e41eb01b297</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>acb2b53b0866d8fb7fb6a3911285944aa</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>aa13ebbff936c3fac16bdf2fc9eea1547</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Expr</type>
      <name>real</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a2954f81f2288e793b7ae9e33e31d9892</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Expr</type>
      <name>imag</name>
      <anchorfile>classcsl_1_1Complex.html</anchorfile>
      <anchor>a1e8118d164e468662c5b2230f24e0f18</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Complexified</name>
    <filename>classcsl_1_1Complexified.html</filename>
    <base>csl::AbstractBuildingBlock</base>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Complexified.html</anchorfile>
      <anchor>a5de9b50162c5af5177a928e855e6b71b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Complexified.html</anchorfile>
      <anchor>a11702661de0ea3b9c59b1f23af0ca0c0</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1Complexified.html</anchorfile>
      <anchor>a9d3fec789b00de1625df53232da4cf30</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Complexified.html</anchorfile>
      <anchor>a05b65b2f6cdbc13a1d757a3f90ae7c0c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Complexified.html</anchorfile>
      <anchor>ab70d880a6d5eaf52b7d365c9d0c29f86</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Complexified.html</anchorfile>
      <anchor>a10b02cb2da960264c19926a76a859488</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Constant</name>
    <filename>classcsl_1_1Constant.html</filename>
    <base>csl::AbstractLiteral</base>
    <member kind="function">
      <type></type>
      <name>Constant</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a663cabfe9fc80e22f4e4b6e5de80da27</anchor>
      <arglist>(const std::shared_ptr&lt; ConstantParent &gt; &amp;t_parent)</arglist>
    </member>
    <member kind="function">
      <type>std::string const  &amp;</type>
      <name>getName</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a8ab390edad6e1237057a217800526862</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>adfbd3cd70f84232b7518ff59b22ed0e6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a81c9c1235cc4b7a3d799a3581cbda675</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>afa00a556bebd39436c680106e79e5d96</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Parent</type>
      <name>getParent</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>af7a592da98893ac5bfc3b145ace1fa01</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setName</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>aa1d8e4b670f8bf97a1253a9056178248</anchor>
      <arglist>(const std::string &amp;name) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCommutable</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>ad3f5b9cfa86f3c63c75fc48669c80c03</anchor>
      <arglist>(bool commutable) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getValued</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a8e05cf80cbeffe60e0e09bc9d15001bd</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>getValue</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a96386d249c7171141d18c9b48cfa5a33</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a93af5fcd73748abeb661cd37206902d2</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a4ce673bdd9773d4837b386ae44611fb0</anchor>
      <arglist>(long double t_value) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a3fa4bb061f59a9da7278b3b0188c7236</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a606d0d84686fe39def7bfed9bacd5817</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>ab324b60959cde778500b675b3bf6433e</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a77a25c084010808eae31429df43996eb</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>ac7602a9ee61e0821667fa6264c003a0b</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a969cc1cbf65be1a69d2693911b3c4617</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator=</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a6cef630b0f3804631f60b6ed22bd3ccc</anchor>
      <arglist>(double t_value) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Constant.html</anchorfile>
      <anchor>a3a9305425d1eeb5c684f9c91b97865ac</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ConstantParent</name>
    <filename>classcsl_1_1ConstantParent.html</filename>
    <base>csl::LiteralParent</base>
    <member kind="function">
      <type>cslParent::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ConstantParent.html</anchorfile>
      <anchor>a763c63b7aba66dc122600c0649a75c9a</anchor>
      <arglist>() const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ContractionChain</name>
    <filename>classcsl_1_1ContractionChain.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Cos</name>
    <filename>classcsl_1_1Cos.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Cos</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>a9d1c44e33f37025e9541d5f9df6e2f24</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Cos</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>a652a02ad7665e479d2ff28e97ceade26</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>a1fb5b606bba498a4ae74ea3ea6f669a3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>addc14896700e9950baee3374a09b558d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>adfb13db23ecb0b199fd19ce03a68af09</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>a7f26952b1462ae3b1cd2ee13ce5dcaff</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>aae93b65d1aa61e9c6a4422d5c168f617</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>afa1a771ec452b284b470682fcb87b002</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>a0004c95693419ba41ad203caf20a7f64</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>a6eb534b4b475261522c8c93ce594646c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>a3baf3fecf9ea48db4ba1fbbfd3aee05b</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>ae4305e350e7d257c189df1d051169f9c</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Cos.html</anchorfile>
      <anchor>a8a840d5af4c79bf20430b144ee0da9b1</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Cosh</name>
    <filename>classcsl_1_1Cosh.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Cosh</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a7bed0807549cc4ed37f89d17b75bd79b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Cosh</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a53d1a15add0bfcd03b16e12fba2917e7</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a49c09e3519a2307ef7c8713c643249ef</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a3d88db8dd64a46ef1cc70b1a0d19bca3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a86f7a0850d4ee1968733701c27273fce</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a1eead10ed28b194fd75399d748a63254</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a6efd3054c14922af72c0e057395869f2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a01a5c90284d2e60271aec223efed3348</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a4f968c087cdf5402e49b67ea3ac5bf19</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a403d11f101661cb030e72f41907fdfc7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>ad40b853fb9959e63b514ed9c71b5bf2a</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a028b7c99b3703dd1fb12c47cada7aaaf</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Cosh.html</anchorfile>
      <anchor>a1ad8069e9a1236eb00551780684b1240</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>counter</name>
    <filename>classcounter.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>counter_iterator</name>
    <filename>classcounter__iterator.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>csl::DeltaParent</name>
    <filename>classcsl_1_1DeltaParent.html</filename>
    <base>csl::TensorParent</base>
    <member kind="function">
      <type></type>
      <name>DeltaParent</name>
      <anchorfile>classcsl_1_1DeltaParent.html</anchorfile>
      <anchor>a2ae0e29347f0b02e7b1c31d95ea3a91a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>cslParent::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1DeltaParent.html</anchorfile>
      <anchor>a5f0c9c1d6c0fd4a25cc2e21a0681efa7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasContractionProperty</name>
      <anchorfile>classcsl_1_1DeltaParent.html</anchorfile>
      <anchor>a8e820c17bda11df8e98eed50e013401a</anchor>
      <arglist>(const Abstract *self, Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>contraction</name>
      <anchorfile>classcsl_1_1DeltaParent.html</anchorfile>
      <anchor>a4736cb2c3631a1a43bd3229fc246d79d</anchor>
      <arglist>(const Abstract *self, Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator()</name>
      <anchorfile>classcsl_1_1DeltaParent.html</anchorfile>
      <anchor>aa0b6e8538707f3621caaadc29d2145c7</anchor>
      <arglist>(std::vector&lt; Index &gt; indices) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Derivative</name>
    <filename>classcsl_1_1Derivative.html</filename>
    <base>Operator&lt; AbstractDuoFunc &gt;</base>
    <member kind="function">
      <type></type>
      <name>Derivative</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a3ba5f37d1f5bfc673d2a8faf2384e8aa</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Derivative</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>aa683d5739f99262187193111cdc5f835</anchor>
      <arglist>(const Expr &amp;variable, int order)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Derivative</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a9a2c938144e1f8381ec017d6dc342408</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Derivative</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>ac88dd0f71b680dae8477aa513b4bb1e3</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, int t_order)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>afede83f7ca9724579f293d4289857d00</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getVariable</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a257f22195ee64163e7ff5ee99b6ecdc1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getOperand</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a08574b1aa7967ca4f6b98f4ed9534bc0</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getOrder</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a8debf6f0fe771b1c0fa0280faf0f3283</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOperand</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>aef758e6375f3faf33875d9a1228cec44</anchor>
      <arglist>(const Expr &amp;t_operand) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a4aacae07230cb240ff9349498cfc339a</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a8beb96c8fb56648031f7df13194f8eaf</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a2cdecc31aec95ae92fdbb0a24f3f8589</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a5165c55343cf326d361e7f4c44df986f</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>af13a07941f0d3c519aca857daa01ee10</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a7d906b1b3929000ec0a6100bb5957697</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Derivative.html</anchorfile>
      <anchor>a13a0500ee86e1ba0f316d6a4d5f4d1ae</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>csl::LibraryGenerator::DiagonalizationData</name>
    <filename>structcsl_1_1LibraryGenerator_1_1DiagonalizationData.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Diagonalizer</name>
    <filename>classcsl_1_1Diagonalizer.html</filename>
  </compound>
  <compound kind="class">
    <name>Diagonalizer</name>
    <filename>classDiagonalizer.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::DiracDelta</name>
    <filename>classcsl_1_1DiracDelta.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>DiracDelta</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>ab2b01b60ad7a45c8b9c94a5b30f43316</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>DiracDelta</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>a5bddad79646bb34aabed6b0ff28bad46</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>afde4d766b81d980647c55bda65ed519a</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>af86344bc4071f901591cc1b50b915257</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>aea8385f39d0ddccf6b45caa1942ec3cd</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>acb4f35b6aef57ce0426f581f65ad7d68</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>a3dd7b9fb46af05be5d43baa83412ebac</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>ac5117381383dfaeb706b5d19b9971014</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>aa3b109eb3dc25e8084df99b1f77fada4</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>aa5b3c23a06e1de1af366eac1470784d6</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>a7a0264b21f4abad295c61e89ead32b88</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1DiracDelta.html</anchorfile>
      <anchor>aa66032c002b1c5d95bb4499093fa19cc</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::EpsilonParent</name>
    <filename>classcsl_1_1EpsilonParent.html</filename>
    <base>csl::TensorParent</base>
    <member kind="function">
      <type>cslParent::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1EpsilonParent.html</anchorfile>
      <anchor>a24dcb0f019f939c6e7a04c7dd1285b06</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasContractionProperty</name>
      <anchorfile>classcsl_1_1EpsilonParent.html</anchorfile>
      <anchor>a6137c073fe9806ab8d0dc8606aec98ad</anchor>
      <arglist>(const Abstract *self, Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>contraction</name>
      <anchorfile>classcsl_1_1EpsilonParent.html</anchorfile>
      <anchor>af19206790efe47aa7529fef3e8b34a7f</anchor>
      <arglist>(const Abstract *self, Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator()</name>
      <anchorfile>classcsl_1_1EpsilonParent.html</anchorfile>
      <anchor>a69dcb179ab67784a6520b95d196747b8</anchor>
      <arglist>(std::vector&lt; Index &gt; indices) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Exp</name>
    <filename>classcsl_1_1Exp.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Exp</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a35a8d2caa991252f7e5f342cdf1d0e44</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Exp</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>ab27539fbf2392ef9682b695cd507022d</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a6e36f13b9963e395c6d9ce5f58fbbaa1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a3e803100efb959d15e40b0e69aa9a369</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a08b474f3f0dbf2255e4c142ad5e095e1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a6fd77dde2c403a67d2020966a14b0033</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a9834c3827d1af36549c055f3a4f65c6d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>acc132fb2a3e66943918158e1be8b2f92</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a4bf08517c64b660eb82cf2922e893cfc</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a396b88644ca6fe350fff11960592c4b7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a9927422cac69f3ef722551ac4d37d33f</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>aff1fdef24f13fcc5f8ccccb8152eaa63</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a53d5fa8b9bf211862d9c6dde6fcad3a6</anchor>
      <arglist>(bool full=false, bool inplace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a0742b26f0efd5a82473f3d8b26524fcd</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inplace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Exp.html</anchorfile>
      <anchor>a074d3618469cd4ab010d6c48c47f4976</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>csl::ExpansionChain</name>
    <filename>structcsl_1_1ExpansionChain.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Expr</name>
    <filename>classcsl_1_1Expr.html</filename>
    <base>shared_ptr&lt; csl::Abstract &gt;</base>
  </compound>
  <compound kind="class">
    <name>csl::Factorial</name>
    <filename>classcsl_1_1Factorial.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Factorial</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>adce06238daf6aca42580f331463674f1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Factorial</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>abc023ba6e9d2f9d45ae7873fa0e4eed0</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>ac31a3e15b4bf0d7604bbdadd2080cc7f</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>ae5625935a6d612978db8b77d6b7a324e</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>a0f94e896bd5032efea33a4faae931980</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>ab104048896f3aafb93bf35a73d8fa783</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>a59bdb12cae8d39b453b2f345f4a388ad</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>aff04d4330b43f2a0c54d1f2089d09e97</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Factorial.html</anchorfile>
      <anchor>a687d7064e35ad99d4981e940c509440d</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>csl::LibraryGenerator::file</name>
    <filename>structcsl_1_1LibraryGenerator_1_1file.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Float</name>
    <filename>classcsl_1_1Float.html</filename>
    <base>csl::AbstractNumerical</base>
    <member kind="function">
      <type></type>
      <name>Float</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>afc5e54b01aa2895dfbda343c13718164</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Float</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>ae220be59f0cf4932908492e3d9179426</anchor>
      <arglist>(long double t_value)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a23bf3dff3c0506d854366653bb9a177d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isInteger</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a2588061b9db6b5af2253bc181b859ef8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a42a8b3b242e0fb3e9b19f10f14bb21a4</anchor>
      <arglist>(long double t_value) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a507979a5b36edb4e3f076f75ba010e7c</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a19efbc737f9188703d55730839b744c0</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a244fc461c1b78ecdf0791d64cef8100d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>ab4d0c0ffa060f4a4242fa1920b5f7c7e</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>multiplication_own</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a0acfdd639afdbbb6f904a2cd74aaac31</anchor>
      <arglist>(const Expr &amp;expr, bool side=1) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>addition_own</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>ac269f085d5e4f9f43d4a5500751cfe33</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>division_own</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a7a35025055eaba4cc64707ac79e2aabc</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>exponentiation_own</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a6a20dfce78e1b4a979d11731db030987</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>ad8c19d92099f26157248fc9023f9f599</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a76dda7f710c690c2a950d2279c6eae3e</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>long double</type>
      <name>value</name>
      <anchorfile>classcsl_1_1Float.html</anchorfile>
      <anchor>a05e5f76e26b313dd420fe760b5fc4430</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Functional</name>
    <filename>classcsl_1_1Functional.html</filename>
    <templarg>Args</templarg>
    <class kind="struct">csl::Functional::Replacer</class>
  </compound>
  <compound kind="class">
    <name>csl::HighDTensor</name>
    <filename>classcsl_1_1HighDTensor.html</filename>
    <base>csl::AbstractVectorial</base>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1HighDTensor.html</anchorfile>
      <anchor>a0434d931a5d7533c32b7b4dc359c24e1</anchor>
      <arglist>() const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Imaginary</name>
    <filename>classcsl_1_1Imaginary.html</filename>
    <base>csl::AbstractLiteral</base>
    <member kind="function">
      <type></type>
      <name>Imaginary</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a009e06a015e3d11f6e6ac55dc893ee55</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a45d77fd51e45fade354f8ec14c303ef7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>add1fe835290daf5c85d3850931351fef</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a3f78df436661ecce2ebfdd97324435c7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>ae7380d519a460d8cf1dff4f676b9df78</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a6a4442943faf0d148692e6f3f70823b2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a4a8beac2eb84273afdfbb771918f04e3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a34b99b842894045a09aa9c796f9d765a</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a78ac0adf5aa6b8c4a159295ca8dd2fbb</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a6c195a0913b72cd58940bd06fedd8432</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>aac1fdac38ead90cfe0d4033896cfb13a</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>a41cff9496eaf914c3a55a2ded645d334</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Imaginary.html</anchorfile>
      <anchor>aa98d68b8591062b56a67452e4d84dd2b</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ImaginaryPart</name>
    <filename>classcsl_1_1ImaginaryPart.html</filename>
    <base>Operator&lt; AbstractFunc &gt;</base>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>a2cb758975b92a34387f83895e261defe</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>a4644adb36c50f172991f587ab0eef89e</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getOperand</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>abaabfff6c5bbda6ce39627bf632d380d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOperand</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>a6e749b2756439d060e103585a11af231</anchor>
      <arglist>(const Expr &amp;t_operand) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>acc310b40a49a21b15ba3a613100d6fdd</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>af9c1420abf4c1060ce30061b75323899</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>a8423033a6934636a15e47b5ccb0df70e</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>adfe21172f295b803952c5541750b338e</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1ImaginaryPart.html</anchorfile>
      <anchor>a52a90d231dbb0518eebb65e5f21e24eb</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>csl::LibDependency::IncludeType</name>
    <filename>structcsl_1_1LibDependency_1_1IncludeType.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Index</name>
    <filename>classcsl_1_1Index.html</filename>
    <member kind="function">
      <type></type>
      <name>Index</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a31beddb6689707568436b5b4686d6191</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Index</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>acba7aec490fd0545a106c3ae7b339a3e</anchor>
      <arglist>(const Index &amp;index)=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Index</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>ab60425c3d248dca9c0f948d316c300a3</anchor>
      <arglist>(const std::string &amp;t_name, const Space *t_space, unsigned short t_id=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Index</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a29d82ab0db436e51aff9cbc9fd44a950</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>char</type>
      <name>getValue</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a27cdc5e75ad694f15e222adc1e594ede</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getSign</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a12ab0df28c86b1700249223856f51955</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getFree</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a7484c23ea4394c83d7861f152741761f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>cslIndex::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a6cd5393db537bf41b6f730e5b40ec18e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getMax</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a82248c179583d3afcf0b636b26851b8a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const Space *</type>
      <name>getSpace</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a0fa29241464306bb6a909a28ab55c8ce</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>getFlipped</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a8de68cd26aaaad9461a0fabf406a896d</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setName</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>af64ff9c829eaf955c13040de627e8058</anchor>
      <arglist>(const std::string &amp;t_name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSpace</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a15378a772f015c44d734a649a57b3aa7</anchor>
      <arglist>(const Space *t_space)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>ab3bac6ffae99d928c5c5c39b9e632487</anchor>
      <arglist>(char t_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSign</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a43197b30091bdcfecf6355fdd765823a</anchor>
      <arglist>(bool t_sign)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFree</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a991575743743aa348dc53058ca87d8e8</anchor>
      <arglist>(bool t_free)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setType</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>aa92ca3061dfcdc7c828a8044e12470b9</anchor>
      <arglist>(cslIndex::Type t_type)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>testContraction</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>af52ce7b7ff09d456c32662510bb33bf7</anchor>
      <arglist>(const Index &amp;t_index) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>ad696266fdaeac8fa5192819f6ec6a11c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a9f93460cbeb33917e898d981f11a7592</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index &amp;</type>
      <name>operator=</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>aade93e0ae2b90e0ae9398f7677bca829</anchor>
      <arglist>(const Index &amp;index)=default</arglist>
    </member>
    <member kind="function">
      <type>Index &amp;</type>
      <name>operator=</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>af819e8b27e3517fd09e7d135093cf554</anchor>
      <arglist>(int t_value)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a3423a6f82a350eab6a7e2f3a039cb337</anchor>
      <arglist>(const Index &amp;t_index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithoutSign</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>afd66d987486da051ca91506fd706a124</anchor>
      <arglist>(const Index &amp;t_index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>exactMatch</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>af60770e33d69e0e10f2a20de65ca27ae</anchor>
      <arglist>(const Index &amp;t_index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a1b3c1bd4ff95621a2f4f8d7e25991239</anchor>
      <arglist>(const Index &amp;t_index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a3b50e99c073627803d1522a0ff6f7fd9</anchor>
      <arglist>(int value) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a79708fb1d23ea07f155b8bfe4502936c</anchor>
      <arglist>(size_t value) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a49a9575712aaf0d2c42de41567400052</anchor>
      <arglist>(const Index &amp;t_index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator|=</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>aa4deba8dfa05e68c7f205ec9859cc7be</anchor>
      <arglist>(const Index &amp;index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&amp;=</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a9fb9b017d2cceb247873636f68e3123d</anchor>
      <arglist>(const Index &amp;index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>aff25bc9f3d911e1cc54460ef08853dd2</anchor>
      <arglist>(const Index &amp;index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>ab2aba99da8f9c26f097adc5a28c297b6</anchor>
      <arglist>(const Index &amp;index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a7d16e34fa4a70163fca0fbb82db0d729</anchor>
      <arglist>(const Index &amp;index) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a9426ee1b6b6a81b0921666729a8ef97c</anchor>
      <arglist>(const Index &amp;index) const</arglist>
    </member>
    <member kind="friend">
      <type>friend std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classcsl_1_1Index.html</anchorfile>
      <anchor>a0cf84d0a24300d30763bd418b380fe92</anchor>
      <arglist>(std::ostream &amp;fout, const Index &amp;index)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::IndexedSymmetry</name>
    <filename>classcsl_1_1IndexedSymmetry.html</filename>
    <base>csl::Symmetry</base>
  </compound>
  <compound kind="class">
    <name>csl::IndexStructure</name>
    <filename>classcsl_1_1IndexStructure.html</filename>
    <member kind="function">
      <type></type>
      <name>IndexStructure</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a1817657d1092dfa1e0aa527c14d0bfd9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IndexStructure</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>af95d656eb01c36b15dc88ce58c061299</anchor>
      <arglist>(int t_nIndices)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IndexStructure</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a669e82c845119d8641782809041abe0d</anchor>
      <arglist>(const IndexStructure &amp;structure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IndexStructure</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a80568cc0626c67cbd0ed965a7d0285a2</anchor>
      <arglist>(const std::initializer_list&lt; Index &gt; &amp;structure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IndexStructure</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a9aaf2bcd47023aa86ec98fd6ba2c7abe</anchor>
      <arglist>(const std::vector&lt; Index &gt; &amp;structure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~IndexStructure</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>adeac74906af00b0c30cb2ceaf5b9c919</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Index &gt; const  &amp;</type>
      <name>getIndex</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a7a43b20b344b5a73fd77410c7e53f303</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Index &gt; &amp;</type>
      <name>getIndex</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a2aeb033ba5546cb3c4e7037724582f06</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Index &gt;::const_iterator</type>
      <name>find</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a39bcded59b10e37cb5eea86593c12dfb</anchor>
      <arglist>(const Index &amp;t_index) const</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getFreeStructure</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>afc69b0341180adbe495dbfc263d4f1f4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getSinglePermutation</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a7f5591d96374516da7a45cc39588f4e7</anchor>
      <arglist>(int i1, int i2) const</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getPermutation</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a9fa67e502d066fd48fd0ad24e51d2c3d</anchor>
      <arglist>(const std::vector&lt; int &gt; &amp;permutation) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setIndex</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a807c597f51589279e55adfb5d0ca911c</anchor>
      <arglist>(const Index &amp;newIndex, int iIndex=0)</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure &amp;</type>
      <name>operator=</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>af773ea9c2619f3dc4bf5c11b309cdf3f</anchor>
      <arglist>(const IndexStructure &amp;structure)=default</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure &amp;</type>
      <name>operator+=</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a1cba6b632b5c34f8a5c49f46f9502c34</anchor>
      <arglist>(const Index &amp;newIndex)</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure &amp;</type>
      <name>operator+=</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>af01e15e1cc969900350097c974fc385d</anchor>
      <arglist>(const IndexStructure &amp;structure)</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>operator+</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a87778aebc1b4900359b3da6ddbaa3afa</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>exactMatch</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a0909806149cbf6b1144ec90d881a37e6</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a0594b0c94c9433ba005d00fc182969ab</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a7352596f7fdaa1a7381025ed0285aa39</anchor>
      <arglist>(const IndexStructure &amp;structure, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a6f6915d5ac3b48e9420614ae4c01a4e2</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>aab0263a3fafae7d082908712c826ac22</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator|=</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a173edcc647a335f0dd682e4bde87763e</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&amp;=</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>adbfd5008fbfa7d39a2e9d53de817b7f0</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a9f837c7c76a35af97dc5d89f9334b868</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a74ecd80db2c44ae29e401f53d9a4d241</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>aed9be5905542246b88f1ee46a189b480</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a72bdcf7919340249e47613ba60d2a5be</anchor>
      <arglist>(const IndexStructure &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a2dec5d53133432e738eca61a952f509c</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>Index &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>ab5863a1c20705db40b424752b6f44dbd</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="friend">
      <type>friend std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classcsl_1_1IndexStructure.html</anchorfile>
      <anchor>a962c1527a6e472f0f48e3a6702e610b3</anchor>
      <arglist>(std::ostream &amp;fout, const IndexStructure &amp;structure)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::IndexStructure_new</name>
    <filename>classcsl_1_1IndexStructure__new.html</filename>
    <templarg>N</templarg>
    <member kind="function">
      <type>bool</type>
      <name>exactMatch</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>a15458bcf88028e33bcd24c3c562caf2d</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>a61d9c10da9b35e1cd84064a78c5fe4db</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>a110cbcd8abf5732286f3db213d3e8c55</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>ac77392f6308a57c666ae2bf95149b3ad</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>a3b7b33d5fe8b47b3b1a1290793b443ed</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>a14838e7d024b1ac7cb2bbc700308c54b</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>a6ea08ba67f5181600d664e891b8763ee</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>a025358a09fd8058d006d169066fd8e3b</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>classcsl_1_1IndexStructure__new.html</anchorfile>
      <anchor>aea35cdda8aeb2aeee6897ff67348c086</anchor>
      <arglist>(const IndexStructure_new&lt; N &gt; &amp;structure) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::IndexStructureView</name>
    <filename>classcsl_1_1IndexStructureView.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::InitSanitizer</name>
    <filename>classcsl_1_1InitSanitizer.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>csl::Integer</name>
    <filename>classcsl_1_1Integer.html</filename>
    <base>csl::AbstractNumerical</base>
    <member kind="function">
      <type></type>
      <name>Integer</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a36186701678cc62a89eef1595d81b66a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Integer</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a54d83e1707273d9e5f758813c24720ad</anchor>
      <arglist>(long long int t_value)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a93751bbb2c3673de15a6a4f7f6f138ff</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isInteger</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>ac5571ab9be29c389391e69d8794b93a8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a5ce6964933d7a9a7df7e8e4254b446be</anchor>
      <arglist>(long double t_value) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a1c8d479f0f4fee521934707dafd162ab</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>ab775bd9a73cd2a9c410e1fdc02c8b9b9</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a3b0670d4d8bddf30871f72a15a30f856</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a029be2af666fd37d37412473cceeb546</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>multiplication_own</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a69ec91334a0491aadfebc431d2174343</anchor>
      <arglist>(const Expr &amp;expr, bool side=1) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>addition_own</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>ad00b35bd5e52a36445b6e99265f20240</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>division_own</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a98edda5fd2b84eb6fb6a5c0000d4c4fe</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>exponentiation_own</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a26e655bb92db0fad0c3970746c8e5c93</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a341c2f0b7aa9cd523ff5ee48c9ba49f0</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a000e1bcdac9556d30acbd0a3059308cd</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>long long int</type>
      <name>value</name>
      <anchorfile>classcsl_1_1Integer.html</anchorfile>
      <anchor>a23724af0e10c950fb77775b360634f1d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Integral</name>
    <filename>classcsl_1_1Integral.html</filename>
    <base>Operator&lt; AbstractDuoFunc &gt;</base>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>af4eedf08f912f9fbb576a01778eecaea</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getVariable</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a0a44c0ebf7c356c5ee7ac288b471edb9</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getOperand</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a304552a9becfb10cb337210117de575c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOperand</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>af49dcf27651f1e5e149093da246dae8f</anchor>
      <arglist>(const Expr &amp;t_operand) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>ade5de0026bdfc743a5574f091340171c</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a6c9ccd94f57b8319977903d7fb0b86e6</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>suppressTerm</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a0c07c13fb5d171702866ce1a7418b166</anchor>
      <arglist>(Expr_info term) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a058cb4c2238b8ff1c66908c80d21fd8f</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a1c9d0d773d2fcc59eaf1e72dd55695df</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a75d07b7ec8e8ede6c9c15157e6a4b47f</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>ae3d145e84f749ba49f57d59a277742c5</anchor>
      <arglist>(Expr_info var) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a467c09b5b03f5e5a6ee28410ded0d09f</anchor>
      <arglist>(Expr_info var) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Integral.html</anchorfile>
      <anchor>a4e94f3a5f8d5945d04bc23e3258bc413</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::IntFactorial</name>
    <filename>classcsl_1_1IntFactorial.html</filename>
    <base>csl::AbstractLiteral</base>
    <member kind="function">
      <type></type>
      <name>IntFactorial</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>affe79f586ccf458846beac15c91e32fc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IntFactorial</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>aaa0d86c9cf7730c1f5a371e342d83714</anchor>
      <arglist>(int t_value)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>getValue</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>a579d7c365479e561e598195e10da65f9</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>a04137e9d1e68e26f6313264aad3504e5</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>ac9dab3ccc2491163dec9d020078fb281</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>af6af261cc740ac8b4037f5a89599f4ad</anchor>
      <arglist>(long double t_value) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>a334c8a8738b4f3fe1dd9c1c022f7e4a6</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>ab1a3293c37e9e099f6c2e1ec977612ec</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>aa25285ce9c79edb763716c5d903febc0</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>a5cfc7511cbf57edd5f47d3bbde9d0798</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>ab973e4c55cd0ecc72949b97de3029c98</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator=</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>a215a7d3a19fae95048e608c7bb8cac95</anchor>
      <arglist>(int t_value)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>a0df76f746b29d7025b7917cae22179e7</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>value</name>
      <anchorfile>classcsl_1_1IntFactorial.html</anchorfile>
      <anchor>a45a927072fc21e9925ba2e406c886447</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::IntFraction</name>
    <filename>classcsl_1_1IntFraction.html</filename>
    <base>csl::AbstractNumerical</base>
    <member kind="function">
      <type></type>
      <name>IntFraction</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a64af43389b114e1a6769b7ded051b8f6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IntFraction</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a8d7b4363a1ea1e122b2c7d688d14a06a</anchor>
      <arglist>(long long int t_num)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IntFraction</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a97bb53f1f44d7973cf2bd39c8473a119</anchor>
      <arglist>(long long int t_num, long long int t_denom)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>abebe0ae65269c3fdddc7759008cc488d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>getNum</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>aafccd523414a7c7ae8d2a4081bbc76a8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>getDenom</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>ab3c87296f84c24abd4e5d0531b992404</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a3278490e3c91491cc8d0a07cccdc13d0</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>af5517b79f442b4fdc42116e717098cfe</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a73241811979616fa6106f9ba2addd23e</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>aff12a6c06a577d55f295d3d474118a73</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>multiplication_own</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a084df9668bd846370585d44ba07d75a0</anchor>
      <arglist>(const Expr &amp;expr, bool side=1) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>addition_own</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>ac3d446b547d4c8b7d3b5359b87c756d5</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>division_own</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a0dc80b9e319900617056e4ab3c233c06</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>exponentiation_own</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a2184e11c09413bea72653bad45e8f997</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a36c47cf74777eb484bd138d826c23edc</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>ad0e5efc8234d94656457b21ce95b6946</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>long long int</type>
      <name>num</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a882b0045bb535f36a07168e2e7dbe3af</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>long long int</type>
      <name>denom</name>
      <anchorfile>classcsl_1_1IntFraction.html</anchorfile>
      <anchor>a3e5298693263b385b5e9b666e8811543</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::IProd</name>
    <filename>classcsl_1_1IProd.html</filename>
    <base>csl::Prod</base>
    <member kind="function">
      <type></type>
      <name>IProd</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>a1b5a6696cf9b3c05bff2fc0b55c520ac</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IProd</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>af98f5daa6ce50924e3e3da95ca85f7a9</anchor>
      <arglist>(const csl::vector_expr &amp;t_argument, bool explicitProd=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IProd</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>ac37a0c740077c5d3c4bb71c806a27b43</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, bool explicitProd=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>a046bd2f7ea7fb90fdf1dadad1a69af64</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>ae5870d95277493876ab0464e98d30c6a</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>suppressTerm</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>aca4461b782859509b8a9a1deb3846bc3</anchor>
      <arglist>(Expr_info term) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>a9c6895c9d37d17614f54e3baeec96585</anchor>
      <arglist>(const Expr &amp;arg, int iArg=0) override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>ac336baabde24424fc39efd66acc084ca</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>replaceIndex</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>a8f32fd847835d145cc8b4ff062d03c7d</anchor>
      <arglist>(const Index &amp;indexToReplace, const Index &amp;newIndex, bool refresh=true) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>mergeTerms</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>a0ceb2414cba9609088bc2b0461ba2191</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>a55b05e348d5a51d47d119059659b0ccb</anchor>
      <arglist>(Expr_info expr, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>af45bd102ad6a062167330ab8715daf78</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>selfCheckIndexStructure</name>
      <anchorfile>classcsl_1_1IProd.html</anchorfile>
      <anchor>a0d8d2b4422c919a70caf945c1ea35984</anchor>
      <arglist>(bool explicitTimes=false)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ISum</name>
    <filename>classcsl_1_1ISum.html</filename>
    <base>csl::Sum</base>
    <member kind="function">
      <type></type>
      <name>ISum</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>a48d3f521ff839adedeeeb5efa186f316</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ISum</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>a6fd41114415c6e8b1d1d1dcc1fb762c4</anchor>
      <arglist>(const csl::vector_expr &amp;t_argument, bool explicitSum=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ISum</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>a369e229c79e8bbfdbddda6a17c572903</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, bool explicitSum=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>a1b7f9344411c6dbf4e132798833c6a68</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>ab5cffff2ab64b2f15099955102849bdb</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>a04d25076743802b4a5d98f082e3441c6</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>selfCheckIndexStructure</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>a8fa8fcf97a45698574ac3059e591cdfb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>replaceIndex</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>ad3f536e97dbf9f257fc08d6a1ad1d597</anchor>
      <arglist>(const Index &amp;indexToReplace, const Index &amp;newIndex, bool refresh=true) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>a55c31985f0a3ee26f551f28c7b651d1d</anchor>
      <arglist>(Expr_info other, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1ISum.html</anchorfile>
      <anchor>ad11402825b61d8997ce188a9abb30d6b</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::LibDependency</name>
    <filename>classcsl_1_1LibDependency.html</filename>
    <class kind="struct">csl::LibDependency::IncludeType</class>
  </compound>
  <compound kind="class">
    <name>csl::LibEval</name>
    <filename>classcsl_1_1LibEval.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::LibEvalSession</name>
    <filename>classcsl_1_1LibEvalSession.html</filename>
    <class kind="struct">csl::LibEvalSession::Perf</class>
  </compound>
  <compound kind="class">
    <name>csl::LibFunction</name>
    <filename>classcsl_1_1LibFunction.html</filename>
  </compound>
  <compound kind="struct">
    <name>csl::LibParameter</name>
    <filename>structcsl_1_1LibParameter.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::LibraryGenerator</name>
    <filename>classcsl_1_1LibraryGenerator.html</filename>
    <class kind="struct">csl::LibraryGenerator::DiagonalizationData</class>
    <class kind="struct">csl::LibraryGenerator::file</class>
  </compound>
  <compound kind="class">
    <name>csl::LibraryGroup</name>
    <filename>classcsl_1_1LibraryGroup.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::linear_map</name>
    <filename>classcsl_1_1linear__map.html</filename>
    <templarg></templarg>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>csl::LiteralParent</name>
    <filename>classcsl_1_1LiteralParent.html</filename>
    <base>csl::AbstractParent</base>
    <member kind="function">
      <type>cslParent::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1LiteralParent.html</anchorfile>
      <anchor>aa4f13d4a38f4e1eb268b4a391d5e0d73</anchor>
      <arglist>() const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Lock</name>
    <filename>classcsl_1_1Lock.html</filename>
    <member kind="typedef">
      <type>int</type>
      <name>ID_t</name>
      <anchorfile>classcsl_1_1Lock.html</anchorfile>
      <anchor>a45cc1ab000114603f736bf41a933175a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::function&lt; bool(Expr const &amp;)&gt;</type>
      <name>predicate</name>
      <anchorfile>classcsl_1_1Lock.html</anchorfile>
      <anchor>ac0a09608dfb2396fa8b48a33a27e800c</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>lock</name>
      <anchorfile>classcsl_1_1Lock.html</anchorfile>
      <anchor>a4949307447d26c318e81fb9dbf3b96c3</anchor>
      <arglist>(Expr &amp;init, int lockId, predicate const &amp;f)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>lock</name>
      <anchorfile>classcsl_1_1Lock.html</anchorfile>
      <anchor>a82a7a584ea0fc6a7ab18ab643923ba4c</anchor>
      <arglist>(Expr &amp;init, predicate const &amp;f)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>unlock</name>
      <anchorfile>classcsl_1_1Lock.html</anchorfile>
      <anchor>a32bff0dd7b12378551d7c1638ca66583</anchor>
      <arglist>(Expr &amp;init, int lockId)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>unlock</name>
      <anchorfile>classcsl_1_1Lock.html</anchorfile>
      <anchor>a49f7a05c265582b69775d87ae9d7c75c</anchor>
      <arglist>(Expr &amp;init)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::string</type>
      <name>lockNameOf</name>
      <anchorfile>classcsl_1_1Lock.html</anchorfile>
      <anchor>a8c571e092eb5779ead4bf76a098169d2</anchor>
      <arglist>(ID_t id)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Log</name>
    <filename>classcsl_1_1Log.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Log</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>addd0fbc4294eec0b61a6d7b56a45c352</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Log</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a37b1fa435e73b66abb0e51b7da5e943a</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a87bd2f7fce0e7a74fdfcbd438553d4c7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>ab37426baaea4c8e148ee683b488d0fb2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a2c9e7604ea16b86991190c95a48b6819</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a130216294a13120423ff0d742321d4b1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a16aff9ed9a21cb6653deb96048f29225</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a86215b004fb03ac928ba6229d1c97535</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>adb9494e204b723c0401b679cde2b36e3</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a557c51c92ffe849c9f96e9a9f65d39de</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a6007d682abe9cd508476984a051c0844</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a6fa3e9448a30bfb341a7ef9253841240</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Log.html</anchorfile>
      <anchor>a53b05a1dd7c973da172358bb673e4549</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Matrix</name>
    <filename>classcsl_1_1Matrix.html</filename>
    <base>csl::AbstractVectorial</base>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Matrix.html</anchorfile>
      <anchor>ad51abe5758a67621ef124901192dd7a2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>determinant</name>
      <anchorfile>classcsl_1_1Matrix.html</anchorfile>
      <anchor>a8015bb0c18517f1fc9429f8c5b95da38</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>trace</name>
      <anchorfile>classcsl_1_1Matrix.html</anchorfile>
      <anchor>a9b6a6fbd8faebb39f785654de2953bc1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>transpose</name>
      <anchorfile>classcsl_1_1Matrix.html</anchorfile>
      <anchor>a5fe3058c93a9dfe149f742c9b601a9b4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>inverseMatrix</name>
      <anchorfile>classcsl_1_1Matrix.html</anchorfile>
      <anchor>accd49f13248f58c2d65edda3bd6d626e</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>symmetrise</name>
      <anchorfile>classcsl_1_1Matrix.html</anchorfile>
      <anchor>ac6c4ce8256eff3054bba06339ddc8525</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>antisymmetrise</name>
      <anchorfile>classcsl_1_1Matrix.html</anchorfile>
      <anchor>a95c95848700fecd5d7041f5c28877946</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getSubVectorial</name>
      <anchorfile>classcsl_1_1Matrix.html</anchorfile>
      <anchor>a1304350a5c07d93607309144d7a58b9f</anchor>
      <arglist>(int iExcept, int jExcept) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::MetricParent</name>
    <filename>classcsl_1_1MetricParent.html</filename>
    <base>csl::TensorParent</base>
    <member kind="function">
      <type></type>
      <name>MetricParent</name>
      <anchorfile>classcsl_1_1MetricParent.html</anchorfile>
      <anchor>a70157042bb8eab3e011ef7cb5448df99</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MetricParent</name>
      <anchorfile>classcsl_1_1MetricParent.html</anchorfile>
      <anchor>a0a8b39b70996a901adcc241813316e1f</anchor>
      <arglist>(const DeltaParent &amp;abstract)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MetricParent</name>
      <anchorfile>classcsl_1_1MetricParent.html</anchorfile>
      <anchor>aeb55044402c1f9be1acdced724160e7e</anchor>
      <arglist>(const Space *t_space, const Expr &amp;t_tensor, const std::string &amp;t_name=&quot;g&quot;)</arglist>
    </member>
    <member kind="function">
      <type>cslParent::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1MetricParent.html</anchorfile>
      <anchor>a2194d37ed574daf54deba112e482637d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasContractionProperty</name>
      <anchorfile>classcsl_1_1MetricParent.html</anchorfile>
      <anchor>af91920eba4f5346a39a13e393f806526</anchor>
      <arglist>(const Abstract *self, Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>contraction</name>
      <anchorfile>classcsl_1_1MetricParent.html</anchorfile>
      <anchor>a42674b8cb437b52c60adb3338718661d</anchor>
      <arglist>(const Abstract *self, Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator()</name>
      <anchorfile>classcsl_1_1MetricParent.html</anchorfile>
      <anchor>a1c137eb96960fbff8eb9aa9792149bd2</anchor>
      <arglist>(std::vector&lt; Index &gt; indices) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::MultiPermutation</name>
    <filename>classcsl_1_1MultiPermutation.html</filename>
  </compound>
  <compound kind="struct">
    <name>csl::matcher::Node</name>
    <filename>structcsl_1_1matcher_1_1Node.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::ObjectPermutation</name>
    <filename>classcsl_1_1ObjectPermutation.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>csl::ObjectSymmetry</name>
    <filename>classcsl_1_1ObjectSymmetry.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>csl::Operator</name>
    <filename>classcsl_1_1Operator.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>Operator&lt; AbstractDuoFunc &gt;</name>
    <filename>classcsl_1_1Operator.html</filename>
    <base>csl::AbstractDuoFunc</base>
  </compound>
  <compound kind="class">
    <name>Operator&lt; AbstractFunc &gt;</name>
    <filename>classcsl_1_1Operator.html</filename>
    <base>csl::AbstractFunc</base>
  </compound>
  <compound kind="class">
    <name>Operator&lt; TensorFieldElement &gt;</name>
    <filename>classcsl_1_1Operator.html</filename>
    <base>csl::TensorFieldElement</base>
  </compound>
  <compound kind="class">
    <name>csl::OptionalCondition</name>
    <filename>classcsl_1_1OptionalCondition.html</filename>
    <templarg>Args</templarg>
  </compound>
  <compound kind="class">
    <name>csl::Parent</name>
    <filename>classcsl_1_1Parent.html</filename>
    <base>shared_ptr&lt; AbstractParent &gt;</base>
  </compound>
  <compound kind="struct">
    <name>csl::LibEvalSession::Perf</name>
    <filename>structcsl_1_1LibEvalSession_1_1Perf.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Permutation</name>
    <filename>classcsl_1_1Permutation.html</filename>
    <member kind="function">
      <type></type>
      <name>Permutation</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a9bad0619514f9d3387cf03e887e87a10</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Permutation</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a566c7fe4c897cd8cca70f11c22e4bfd2</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Permutation</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a68d93318b33dbcf56c07f02b683fa937</anchor>
      <arglist>(const std::vector&lt; int &gt; &amp;t_permutation)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Permutation</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>aea047cac52ceef3ae36da245a81d50af</anchor>
      <arglist>(int n, const std::vector&lt; int &gt; &amp;list)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Permutation</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>ab37a96afad771439d3c0e7c88f68e684</anchor>
      <arglist>(int n, const std::vector&lt; std::vector&lt; int &gt;&gt; &amp;list)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Permutation</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a4dfe13e034abb2f0c07d7b5740fbdb52</anchor>
      <arglist>(const Permutation &amp;permutation)=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Permutation</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>ae8acae791e3e62bc9fa03880caa625ab</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getOrder</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a784ea8228496eb95cfee1b154706f22f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getSign</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>addbb3b81b418b82e7ffd117481f20076</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getSymmetry</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>aab98401f8dcf633564c05febc2d558fc</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; int &gt;</type>
      <name>getPermutation</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a31b6397f8a71074a3aa3caad9ba4bab2</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSymmetry</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a04828f5c5f1c09113c36fc7e4046004f</anchor>
      <arglist>(int t_symmetry)</arglist>
    </member>
    <member kind="function">
      <type>Permutation &amp;</type>
      <name>operator=</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a3b7cc178049432db72166de18866d017</anchor>
      <arglist>(const Permutation &amp;t_permutation)=default</arglist>
    </member>
    <member kind="function">
      <type>Permutation</type>
      <name>operator*</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a9bfd2d0273205d283c56b76a9ad73e2a</anchor>
      <arglist>(const Permutation &amp;t_permutation) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>ad536ca2c88b4455cca2cc47c631707de</anchor>
      <arglist>(const Permutation &amp;t_permutation) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>aa6130ce3964f0e341334bfb864b1c842</anchor>
      <arglist>(const Permutation &amp;t_permuation) const</arglist>
    </member>
    <member kind="friend">
      <type>friend std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classcsl_1_1Permutation.html</anchorfile>
      <anchor>a433483904207671477236253236b0062</anchor>
      <arglist>(std::ostream &amp;fout, const Permutation &amp;permutation)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Polynomial</name>
    <filename>classcsl_1_1Polynomial.html</filename>
    <base>csl::AbstractMultiFunc</base>
    <member kind="function">
      <type></type>
      <name>Polynomial</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a581525b4e0abcc681f51167bfaf91b7c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Polynomial</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a9fee463f224ef9331c17847e13337eaa</anchor>
      <arglist>(const Expr &amp;expr, const Expr &amp;t_variable)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Polynomial</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a7c8de236dca80022182e05a448ab1de4</anchor>
      <arglist>(const csl::vector_expr &amp;terms, const Expr &amp;t_variable)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>aaba60af7af69c8f95c2a52c9f3669cdd</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getOrder</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>acdfc0c1919d2b4989019e53a80c89f33</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getVariable</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>ad633f5e1ef2f0d4984bb268b65ebafec</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a83502630a7e4016e818c2f8332a68594</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a7990b3e8d7eb1fc76e6270c71eb4a89c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>ace97cf2aeb447514487c478e5b2d5f90</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>af1a0a4d8e8fb9358c90da3ce04d078d1</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a7a0909807aa3cd11e0d213370c742bb4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a4ef2dc0f782257d31ba7f5028cbd5e34</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>ac50287568d1413fc72734a631e1e9f5c</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>abba6b5b6abd6d86b1aa3f51ebd34201d</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getRegularExpression</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a6bc25a8b8e8da0cebdead8cc869e2134</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>addition_own</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>aa3ba91ac2475de7236ee8d48b85fc9fb</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>multiplication_own</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a83e31e00d46a46aaed0789dfc8170a11</anchor>
      <arglist>(const Expr &amp;expr, bool side=1) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>division_own</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>ace27cf683f51c1a50f35e28b9250e5e6</anchor>
      <arglist>(const Expr &amp;expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>ae49aeb195fc59129d3c9e087b32a35b1</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Polynomial.html</anchorfile>
      <anchor>a6ee9ab3e7acf085e35349942cb6c9fc2</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Pow</name>
    <filename>classcsl_1_1Pow.html</filename>
    <base>csl::AbstractDuoFunc</base>
    <member kind="function">
      <type></type>
      <name>Pow</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a8f7d715ce30bfa7f62acbb515e4e5c7f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Pow</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a8ee75567cc4c4a69bf142148f02111a8</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, bool explicitPow=false)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a8d7c1cdb87061dd7aa472ed9fe344a8b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getNumericalFactor</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>aefbb5ef66721e97b90dc1bb82b9e6823</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getTerm</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a71c50f5ed80151b5d75afe6e14164d78</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNFactor</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a5f65eed081f73e0c0ff2c9c5247e5aa9</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr</type>
      <name>getFactors</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>ae28914863f9dfd25c0acdeb372ef2455</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>getExponents</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a6906b6f3c06c1e1c7aad71d272eac65f</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, std::vector&lt; Expr &gt; &amp;exponents) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>askTerm</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>aca1c3fe5c3b30c69807073da07fe9d4e</anchor>
      <arglist>(Expr_info expr, bool exact=false) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>suppressTerm</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a4c86c0033639557a339816bf9a80a9a7</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>suppressExponent</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a3f66770914a5aeb7b83cbfa5f1b9eca6</anchor>
      <arglist>(Expr const &amp;factor, Expr const &amp;exponent) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>ab214090619fd0e99b39b8912f8eed7b1</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>aff8a636e76fdb85a66a389c312ca5729</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>aa2d9132cb7ca63dfba3eb81466a9c3c9</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>aa12ddb1c3b4952756d9eb15196a1674f</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>mergeTerms</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a839d53bdfb2d32b7bb2ca2093f9b9d91</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a01955e262d0a06de299d1debf1e14da8</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>ad5d18f22f9eed16868b1e8e14a0797c3</anchor>
      <arglist>(bool full=false, bool inplace=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a9b2bb69232d43c764369d8859662ecda</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inplace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>isPolynomial</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a52d9ee469cf861e08a58588ce3edf1c4</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getPolynomialTerm</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>a295d149cfcfa20527b12f3bc57e95811</anchor>
      <arglist>(Expr_info t_variable, int order) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>af0de63b31d13243da82cea81cdda9c39</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Pow.html</anchorfile>
      <anchor>ab241ff256f05031465b1d7c959b4056f</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Prod</name>
    <filename>classcsl_1_1Prod.html</filename>
    <base>csl::AbstractMultiFunc</base>
    <member kind="function">
      <type></type>
      <name>Prod</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>ab942e9b327bb3b766a647126faf0937f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Prod</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>ae6bb0fe561daa5930fb62e2dc0fc9c50</anchor>
      <arglist>(const csl::vector_expr &amp;t_argument, bool explicitProd=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Prod</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>aadee4caca89f50c6ed7ad9dce7c69003</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, bool explicitProd=0)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>af0bd5312033ca6006d54e117503cc52b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>acb1ea1810e83874623d07f370efde5c2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a74b08ed837b9be3299e99b5b355cb4d4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>add2e178b9370eba2bd70d3ed17512a0c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>af42fef880d91a3705f0a86065f53560d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>aad33ce50bdda177231353749abb24b71</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>findSubExpression</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a6658c74878dc31902a45b75994baef85</anchor>
      <arglist>(Expr_info subExpression, const Expr &amp;newExpression) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getNumericalFactor</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a31991415ad6e7e8e5e9b703f9128af39</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getTerm</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a428ffb91c3529b4835d6bfa6289765d8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNFactor</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a065a0b483082e58a478016d0821427ab</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr</type>
      <name>getFactors</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>aaa4dca91f1ce4fc9d6f04d528e07db90</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>getExponents</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a1b3dfc8deb0baece750b925ef1a95402</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, std::vector&lt; Expr &gt; &amp;exponents) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>askTerm</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>adb2f82801fb73fb0baaaa18079e6cfa5</anchor>
      <arglist>(Expr_info expr, bool exact=false) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>suppressTerm</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>ad278cc42d80c2429fc3f6cd2abe38b33</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>suppressExponent</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a3b491384c2717ae8b618d91dd8db39f1</anchor>
      <arglist>(Expr const &amp;factor, Expr const &amp;exponent) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insert</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>ae065e3860edfd408a1cdef2bfb259b2e</anchor>
      <arglist>(const Expr &amp;expr, bool side=1) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a6f7a134346d64c86bbc3bf855321a568</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a3bcfa6d3a4b9a8d1ab4255f01fe7edfd</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a3bfdd94f06244aa5af7027901d9e00dd</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>abd8408064c53f614ef5e2bdff399c6d3</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>mergeTerms</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a6fd5885a37a7e6492ec7777121e23944</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a3830459faf91d3cf17c7fa401fb00557</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a2c508afc1d4a74cd1ca69b1f900993d5</anchor>
      <arglist>(bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>aa6bb716b3dea854dd7e897976cf08788</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>isPolynomial</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>ad52e8f95508d38377324e9ef8fb4c28d</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getPolynomialTerm</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>ab552c778033130c31c476b2e1939e84e</anchor>
      <arglist>(Expr_info t_variable, int order) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a68dff5c0b393ad75f5e7e0ac97d229dc</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Prod.html</anchorfile>
      <anchor>a1a2c0f67a8aa5798dd9226d721237a8a</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ProgressBar</name>
    <filename>classcsl_1_1ProgressBar.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::PseudoIntegral</name>
    <filename>classcsl_1_1PseudoIntegral.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::RealPart</name>
    <filename>classcsl_1_1RealPart.html</filename>
    <base>Operator&lt; AbstractFunc &gt;</base>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>a10f26df41d893a03dd72b734469b7bf3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>a0da475d679751428325bac460a651d85</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getOperand</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>a10e389ad0a81beffe8c0a134f4f48847</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOperand</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>a3e065209200a53a0a5d13d4731298832</anchor>
      <arglist>(const Expr &amp;t_operand) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>a139d225aa4b008c2d39e1cbe351e5904</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>a44828a44f3945f853c2b94e4b1e31820</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>aeb57b368c5ae7caa278ca998cb3a4782</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>a1f8b200d737183f15a2edb7d923d464d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1RealPart.html</anchorfile>
      <anchor>adac617913cdfa2354a085002f443f123</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>csl::Functional::Replacer</name>
    <filename>structcsl_1_1Functional_1_1Replacer.html</filename>
    <templarg>Arg1</templarg>
  </compound>
  <compound kind="class">
    <name>csl::ScalarIntegral</name>
    <filename>classcsl_1_1ScalarIntegral.html</filename>
    <base>csl::AbstractIntegral</base>
    <member kind="function">
      <type></type>
      <name>~ScalarIntegral</name>
      <anchorfile>classcsl_1_1ScalarIntegral.html</anchorfile>
      <anchor>afd2cb6972734a809b217d15574cccdac</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1ScalarIntegral.html</anchorfile>
      <anchor>af090ec91b6cf2512df62e86d9b7b032a</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getVariable</name>
      <anchorfile>classcsl_1_1ScalarIntegral.html</anchorfile>
      <anchor>a330a8f285ec3b13b75cd444d88f1fd67</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1ScalarIntegral.html</anchorfile>
      <anchor>a4010ee626fd75e493ac3117fad8d6819</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1ScalarIntegral.html</anchorfile>
      <anchor>a6b5309cffd01f21e5485a61b5bb351f3</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1ScalarIntegral.html</anchorfile>
      <anchor>a86a7f329d187d9abddb819dacf61d5e3</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::ScopedProperty</name>
    <filename>classcsl_1_1ScopedProperty.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>csl::SelfContraction</name>
    <filename>classcsl_1_1SelfContraction.html</filename>
    <member kind="function">
      <type></type>
      <name>SelfContraction</name>
      <anchorfile>classcsl_1_1SelfContraction.html</anchorfile>
      <anchor>a862feb1639cfa1e28ee97726d8c6de58</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, std::optional&lt; std::function&lt; bool(Expr_info, Expr_info)&gt;&gt; t_condition=std::nullopt)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>empty</name>
      <anchorfile>classcsl_1_1SelfContraction.html</anchorfile>
      <anchor>ace6312eed5c6ccb41b6b1d71664b1419</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1SelfContraction.html</anchorfile>
      <anchor>a45abcf33d0e4a277bab9ff02465d34b7</anchor>
      <arglist>(const SelfContraction &amp;other) const</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::vector&lt; std::pair&lt; int, int &gt; &gt;</type>
      <name>contraction</name>
      <anchorfile>classcsl_1_1SelfContraction.html</anchorfile>
      <anchor>aec95d702c5297e89b9bd82ccb7db6307</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classcsl_1_1SelfContraction.html</anchorfile>
      <anchor>a7acb03cf2aa20914ca5deb267c8db387</anchor>
      <arglist>(std::ostream &amp;fout, const SelfContraction &amp;c)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>std::shared_ptr</name>
    <filename>classstd_1_1shared__ptr.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; AbstractParent &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; csl::Abstract &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; csl::AbstractParent &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; csl::ConstantParent &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; csl::LibraryGroup &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; csl::VariableParent &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; TDerivativeParent &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; TensorFieldParent &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>shared_ptr&lt; TensorParent &gt;</name>
    <filename>classstd_1_1shared__ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Sin</name>
    <filename>classcsl_1_1Sin.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Sin</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a55a7bf021a80e54b301e1b3fcc96d4b2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Sin</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a1bfadc977fa061df1733a82f23954422</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a9f090098e2afc33ef5ce79391a207164</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a35ffd397f83edf315fd120c96f44063a</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a84960733192e56a4335215ba5ebb6a65</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a34f3048a1f09a214bc240cb0030c2c11</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a91951762c50bbf889d5d0aa1cfd7847f</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a05bd2fbbf3e80be7bab9a1df28c3f27f</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>ab26f459fab68d5b382ff9a4be70b9c2d</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>af7b3d229ea8b52816102b4facbd7d6bb</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a9977b55ee7079bf9bc7205a57975dd5f</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a4a9faa3552509a110f6182306a3dd66c</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Sin.html</anchorfile>
      <anchor>a51c0924da089969435b2fb45118e9a52</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Sinh</name>
    <filename>classcsl_1_1Sinh.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Sinh</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a6a1efb6bbc82e786c0462aa26bc638a2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Sinh</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a3ca5746ee86d3559ac80309e42d036a2</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a482b3aa41cdfda7b21a08914963e9ea4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a5888952203e8271368e1aa19681afac4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>aa9674cc4b3452c469d0c1314e3c8bd5b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a9c8562fa4fa0a8b9bfd299659b2948cf</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a596d1ee28a2d06a4e4c0bf55d7f12041</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>abeeba07acff9c5b5cc8d2b90d4660ae5</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a1cfab0be84d64aa34ba65ffdc8303937</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>ad2e6606fa0e330acc102e4057a627425</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>aa7b24dd1844a99aa1bf73f59829d6cb0</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a8c235daf099d1ff4ebbff9fa846a7e82</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Sinh.html</anchorfile>
      <anchor>a32616979f93899f873439e54c65ab376</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Space</name>
    <filename>classcsl_1_1Space.html</filename>
    <member kind="function">
      <type></type>
      <name>Space</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>ae2c19f01c2f4178429c9308204fc46c3</anchor>
      <arglist>(const std::string &amp;t_name, int t_dim, std::vector&lt; std::string &gt; const &amp;indexNames={&quot;i&quot;, &quot;j&quot;, &quot;k&quot;, &quot;l&quot;})</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Space</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>ae9a8a760bba6985fd84bc3223056be36</anchor>
      <arglist>(const std::string &amp;t_name, csl::Expr const &amp;t_dim, std::vector&lt; std::string &gt; const &amp;indexNames={&quot;i&quot;, &quot;j&quot;, &quot;k&quot;, &quot;l&quot;})</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Space</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a76e7e8294faef61ec0257b9915b5c8d8</anchor>
      <arglist>(const std::string &amp;t_name, int t_dim, const std::string &amp;name_metric, const Expr &amp;t_metric, std::vector&lt; std::string &gt; const &amp;indexNames={&quot;i&quot;, &quot;j&quot;, &quot;k&quot;, &quot;l&quot;})</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Space</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a8f77196927553767c6ebf2e17afd0a69</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::string</type>
      <name>getName</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a968b828ad5aa203d744a28eb742d957f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getDim</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>ad5f5d81d0a1b9491809fc90f8b6a5545</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>csl::Expr</type>
      <name>getSymbolicDim</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>ac0c47dede17d3902cf375f38950de656</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getSignedIndex</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a6cec2c26132ef1bae61f2c6b012c16f3</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>generateIndex</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a3e22f45ad75d9f625ef0637cd777190f</anchor>
      <arglist>(const std::string &amp;name) const</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>generateIndex</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a57e35dce20e198e9eabab3224061ab07</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>generateVector</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a3a5473e7e5075c398bdd98c2029dfaac</anchor>
      <arglist>(const std::string &amp;t_name) const</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a5492e168d367d61c45c685835a623244</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>dim</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a0bb4f39fe932f6028ba31f7072e94caa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>signedIndex</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>ad73c369af3d12acd80df17f4143b9551</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Tensor</type>
      <name>delta</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>a98c96a51dd8656972534f1890cdc7f88</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Tensor</type>
      <name>metric</name>
      <anchorfile>classcsl_1_1Space.html</anchorfile>
      <anchor>adb37e6c8c611b83ce476a165b65bf978</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Sum</name>
    <filename>classcsl_1_1Sum.html</filename>
    <base>csl::AbstractMultiFunc</base>
    <member kind="function">
      <type></type>
      <name>Sum</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a171ac02d7a8afe0d1242c6307a03acff</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Sum</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a8b45ce85f4650c5ca71675fe5231efae</anchor>
      <arglist>(const csl::vector_expr &amp;operands, bool explicitSum=false)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Sum</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a954e6e01884fce66529f78ee92a21c9c</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, bool explicitSum=false)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>afdde4c81bf81217accd1b02b8e2b4dbf</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>ad9ce7a5183d105402cdafab290b93c91</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getNumericalFactor</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>ae5a42358fbbca3043d216049222455d6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getTerm</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>aa265b9928bffd0aefe456d9dbe3637b8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr</type>
      <name>getFactors</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>ab2e21f4faf128cc9d9e471370def40e7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a340d0daf8560d7d9ec73a30a0e5b4fce</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a91c2be7f869326606fba475ea557b409</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a6a358933e479aa4dcb68c9fff7a4dcc3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>adc1cd9a04f2299205aa8d896ba03482b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insert</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a8967f1bfbae1e49e21bd5f79939b2a63</anchor>
      <arglist>(const Expr &amp;expr, bool explicitSum=false) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a6fff095607bbdb39b0f1d946ef6c20fb</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a98d6f1d250495606344f29e78ba47ee0</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>aa8f5632f8b3be2615f29085195c7abe2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>af3c54e000687a23ad678c77760ffaaf1</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>mergeTerms</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>ab057bcec5aad014c3ac4d1ab847c9ada</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a7dd23134290bcfeffb529e2c03ca17ff</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>af56a4a850dcb91db747ff8ca36f458a2</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>aeb31f0009bdb90913b98c693578b54a3</anchor>
      <arglist>(Expr_info expr, bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>collect</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>af1f3a6d6201fef00ca9d4ad334cd01bd</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, bool full) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>askTerm</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a1620a408e74aa2b452a176f006fb3e5c</anchor>
      <arglist>(Expr_info term, bool exact=false) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>suppressTerm</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>a37d045772a3fbedf76bf72588ee2e212</anchor>
      <arglist>(Expr_info term) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>abd3105d37243fd781e0b2da2de60c295</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Sum.html</anchorfile>
      <anchor>ae8309f4d05302f47272a5e2ba0e6507c</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>symmetricCounter</name>
    <filename>classsymmetricCounter.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>csl::Symmetry</name>
    <filename>classcsl_1_1Symmetry.html</filename>
    <member kind="function">
      <type></type>
      <name>Symmetry</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>a32e5ba2a585c4686fa3ba1992c52cf57</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Symmetry</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>af1d7357f37d7a60a0aa27281b181be33</anchor>
      <arglist>(int t_dim)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Symmetry</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>af856c38edc16c6c1b0765674a7f645b1</anchor>
      <arglist>(const Symmetry &amp;t_symmetry)=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Symmetry</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>aff5592b6996b5a591fb28fee4ceadf0d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getDim</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>ae03ef7f15091cbdfe60f3b126d5bcd8f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>getNPermutation</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>acf7900a021d3305c7b28219f87f5912c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Permutation &gt;</type>
      <name>getPermutation</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>a3733cbdb336ee0c56a5a5558dbe0f519</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addSymmetry</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>a94702e263f2507f1d0a62e44439f91db</anchor>
      <arglist>(const Permutation &amp;newPermutation, int sym=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addSymmetry</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>a5f1e218c4d422f33951eda6a23433cef</anchor>
      <arglist>(const std::vector&lt; int &gt; &amp;newPermutation, int sym=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addSymmetry</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>a85396b261e9d3f1f89c47868fea3280a</anchor>
      <arglist>(const std::vector&lt; std::vector&lt; int &gt;&gt; &amp;newPermutation, int sym=1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>aa5871bff212cd60b52c305ff35041e13</anchor>
      <arglist>(const Symmetry &amp;symmetry) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>ab4ee356a013b40083d495cf0884c0128</anchor>
      <arglist>(const Symmetry &amp;symmetry) const</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>dim</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>a8baa60e7441207afb4a49c9ad9e756c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::vector&lt; Permutation &gt;</type>
      <name>permutation</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>ac796bc32129e793f5d3314b21dfe27df</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classcsl_1_1Symmetry.html</anchorfile>
      <anchor>a04a4c9794260b1c1521da0f56d905505</anchor>
      <arglist>(std::ostream &amp;fout, const Symmetry &amp;symmetry)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Tan</name>
    <filename>classcsl_1_1Tan.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Tan</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a8e10967b6d14a4a568115176d6fe6c53</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Tan</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a55dbab6ad35ec5bf90706fbd7dcf293c</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a13e3eefa59b9a66427401b7a9693cb55</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a89fd2f3ff51edc27812a0440189da5cc</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>af3ffacb8a8af004785a01eb099d5c246</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a34ccf0fdcfa2a43f5725e32e9c201aeb</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a36c5713ba2b02de5da810bbdc1cf0cce</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a6a1e5434ee58f6eb13d4bacf43e97804</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a69fc6716a19363bc3379d64faad91f28</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a01cd6db9fc2543411cba9362d082b54d</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a1c5025345411cb71d081394524492ae1</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a53bc7f2997e0380a6d148c2f1dbf395a</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Tan.html</anchorfile>
      <anchor>a888df766c47e82b50a9e2bd72192cbbb</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Tanh</name>
    <filename>classcsl_1_1Tanh.html</filename>
    <base>csl::AbstractFunc</base>
    <member kind="function">
      <type></type>
      <name>Tanh</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a57afe5b06aa954f10909e90da261ed9a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Tanh</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a962ed085fab97805822cdb5c22194606</anchor>
      <arglist>(const Expr &amp;t_argument)</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a4a30d37ea900d3ce7ac37016bf669bd8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getRealPart</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a64eca21f5e81ff0a85e9d2bbd919edcd</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getImaginaryPart</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a6079ee469f697fb680639537590ebd0f</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexModulus</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>ae5ecb5e85628afc42d25872291a718ed</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexArgument</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a3c66791362233e86cf739e907303d697</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a7f8da1a8e95f15ebd43e291dd0e196d0</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a9ec76b9e7d1c85e6a20f100a3c1e975f</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>ae507f59e44931085e7ac8a7d85a28ee5</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a094ae3e5b29e66a9d4319fb774b5bfcb</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a4a70018c2b554641a3ab51291ca4a878</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Tanh.html</anchorfile>
      <anchor>a33d8c0c83c0e2c9500448ceba8b61428</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::TDerivative</name>
    <filename>classcsl_1_1TDerivative.html</filename>
    <base>shared_ptr&lt; TDerivativeParent &gt;</base>
  </compound>
  <compound kind="class">
    <name>csl::TDerivativeElement</name>
    <filename>classcsl_1_1TDerivativeElement.html</filename>
    <base>Operator&lt; TensorFieldElement &gt;</base>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a16ff4a3a49daf68786abbe407d1d5b8c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a0d83fa5e3869c4e375e4d5d3f0ae8757</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>aa3b01cb3bd1dcef5901cc82ec29c66b3</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a984d8c40c787a0d7350aa0de1969c65f</anchor>
      <arglist>(bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>factor</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>afff58e758180748e03f566e9ac83e728</anchor>
      <arglist>(Expr_info expr, bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>collect</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>aeec6c024c50776cf5439872586451579</anchor>
      <arglist>(std::vector&lt; Expr &gt; const &amp;factors, bool full=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>add5adbf1747cd9343c96dca5a2b7479b</anchor>
      <arglist>(bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>expand_if</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a1fee5554d976f77bf0e2dd5a451890b7</anchor>
      <arglist>(std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, bool full=false, bool inPlace=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a618856543c22406e8c7eed5b8a5a9c65</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>af76375171fcd6824d8410030fb2d5c28</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>getArgument</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a3f571d26e241f9a943e11191c3ebe9a6</anchor>
      <arglist>(int i) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setArgument</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>adce6c89ecb493c05627b42f036e49b5b</anchor>
      <arglist>(Expr const &amp;arg, int i) override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getOperand</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>aba17709ddc3ea5ae260a6b713a032355</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOperand</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>ab78ef96e45ffe45a9fd18b315ad73db9</anchor>
      <arglist>(const Expr &amp;t_operand) override</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>aab730479855c747fa6baafe4c50ca8cf</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setIndexStructure</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>ae976c5be78ef54ca485001a9807d75bc</anchor>
      <arglist>(const IndexStructure &amp;t_struct) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>selfCheckIndexStructure</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a336af14506e8d3877cfc15eeb3eb9149</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>replaceIndex</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>ae304eceb527459e79e9cc635350b1a70</anchor>
      <arglist>(const Index &amp;indexToReplace, const Index &amp;newIndex, bool refresh=true) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a1b6a2e64b07222e4d5d126d2281c4816</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>ac28ceccb0fea566ae54b2d01b70548f5</anchor>
      <arglist>(Expr_info other, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a380f7ce153c1b608c232e86f79bdcbd0</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr const  &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>a2e9000986cc55b7016cc56513f058f69</anchor>
      <arglist>(int i) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr &amp;</type>
      <name>operator[]</name>
      <anchorfile>classcsl_1_1TDerivativeElement.html</anchorfile>
      <anchor>aa2934b8a3734feac07dec2abc1e14e5a</anchor>
      <arglist>(int i) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::TDerivativeParent</name>
    <filename>classcsl_1_1TDerivativeParent.html</filename>
    <base>csl::TensorFieldParent</base>
  </compound>
  <compound kind="class">
    <name>csl::Tensor</name>
    <filename>classcsl_1_1Tensor.html</filename>
    <base>shared_ptr&lt; TensorParent &gt;</base>
    <member kind="function">
      <type></type>
      <name>Tensor</name>
      <anchorfile>classcsl_1_1Tensor.html</anchorfile>
      <anchor>a67324d06a4976f94ae32774e8354f1d0</anchor>
      <arglist>(const std::string &amp;t_name, const Space *t_space)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Tensor</name>
      <anchorfile>classcsl_1_1Tensor.html</anchorfile>
      <anchor>aaf9c1c4ea203812b3c471b09a723d654</anchor>
      <arglist>(const std::string &amp;t_name, const std::vector&lt; const Space * &gt; &amp;t_space)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Tensor</name>
      <anchorfile>classcsl_1_1Tensor.html</anchorfile>
      <anchor>ac28a258f45cfcb02569e081631322ae1</anchor>
      <arglist>(const std::string &amp;t_name, const std::vector&lt; const Space * &gt; &amp;t_space, const Expr &amp;t_tensor)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Tensor</name>
      <anchorfile>classcsl_1_1Tensor.html</anchorfile>
      <anchor>a8078fbe522573aa796133442f67b0065</anchor>
      <arglist>(const std::string &amp;t_name, const Space *t_space, const Expr &amp;t_tensor)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::TensorElement</name>
    <filename>classcsl_1_1TensorElement.html</filename>
    <base>csl::AbstractElement</base>
    <member kind="function">
      <type></type>
      <name>TensorElement</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a1b29839ab99dd106e336ce900023d18a</anchor>
      <arglist>(const Index &amp;t_index, const Parent &amp;t_parent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorElement</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>ac2169c4eabe165be2f0c455f658a0036</anchor>
      <arglist>(const std::vector&lt; Index &gt; &amp;indices, const Parent &amp;t_parent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorElement</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a3bba3e8ce7e864a2921ad91e4019454c</anchor>
      <arglist>(const IndexStructure &amp;indices, const Parent &amp;t_parent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorElement</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a4d45c8d308213ea65d46f9aeab883d9f</anchor>
      <arglist>(const Abstract *&amp;expression)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorElement</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a4f78788e311f00a646e933b102fe365c</anchor>
      <arglist>(const Expr &amp;expression)</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a1c246057031fbc18868bac66d13be37c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>acc36470e69d6edf43f6dd7b6db0365d2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNIndices</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a5ddd03efb94ea0017def76cd6777f745</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a3dc05ca45a03e8a7f8459812e07b85f6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>acd1ac409b891dd033c1792f5473d8d10</anchor>
      <arglist>(Expr_info expr, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const override</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a7b99f83fdff13fb508ea8e671d4dc834</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>getComplexConjugate</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a684ed9584f9bbe4ffc1e842b84a05ed0</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>getIndex</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a029c4358822daed54f4f775032896790</anchor>
      <arglist>(int i) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>askTerm</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a6ec375934423dbb60f6d00ea13ec7f80</anchor>
      <arglist>(Expr_info expr, bool exact=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a14ad4f8a47af9ea009bcea3385e45bc3</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a22a5c9686859930af78f51a85b31160d</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>checkIndexStructure</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>ad9d1474eb8d1c53835ffb3891e01b020</anchor>
      <arglist>(const std::vector&lt; Index &gt; &amp;t_index) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>replaceIndex</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a02fe69a766b8eade9d5de6cbea8054a6</anchor>
      <arglist>(const Index &amp;indexToReplace, const Index &amp;newIndex, bool refresh=true) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setIndexStructure</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a387f69126ae40d711755945ef023de92</anchor>
      <arglist>(const IndexStructure &amp;t_index) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasContractionProperty</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a511992d33dc54c1fb8d0c4c17afa5f8c</anchor>
      <arglist>(Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>contraction</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a2a301cd1beb0eecde2b83c1ca6167c59</anchor>
      <arglist>(Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>applyPermutation</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>ad6f100f8c15bee5c29d97d40549a6e18</anchor>
      <arglist>(const Permutation &amp;permutations) const</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr</type>
      <name>getPermutations</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>aa0f1ab4f63aa28f3c759a666fb15636a</anchor>
      <arglist>(bool optimize=true) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a0b5baffa4b563fd8e2e8ec4ee00446b1</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a5e4794abc02ba5d8f80f76f64e9cab9c</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a7a1fc7737cce9da527d3796d2fd711d8</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>aaaa0e8d228945bad2ef6a8474a6df0c0</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a6ada509cc19d2040553f8f40621914db</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>IndexStructure</type>
      <name>index</name>
      <anchorfile>classcsl_1_1TensorElement.html</anchorfile>
      <anchor>a7f7b50e1bbd4157e8b2797740aa31b89</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::TensorField</name>
    <filename>classcsl_1_1TensorField.html</filename>
    <base>shared_ptr&lt; TensorFieldParent &gt;</base>
  </compound>
  <compound kind="class">
    <name>csl::TensorFieldElement</name>
    <filename>classcsl_1_1TensorFieldElement.html</filename>
    <base>csl::TensorElement</base>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>aab050fd18460d4842d8972a990363b0c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>ab78b88ff1b49707ee98aee3211214044</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>a9bc54de57d7301838f673c014ed9d79f</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>afac7a7dd245d9d1bc5f52700e2a6acb8</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>ac36fad3d18e46c353b4d961867d257cf</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>a55ad56ca57e69bdcb5b77e6d0bf254c7</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>commutesWith</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>aa9de316c2975e097ea77162d7d1040c8</anchor>
      <arglist>(Expr_info expr, int sign=-1) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>ab452e8d298be5e58e28e92aa6eca1c25</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>a43644a7030777b98a28cbaa15f229bfa</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>a7568ac709f768fc70a4797731b7d9e7a</anchor>
      <arglist>(Expr_info other, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1TensorFieldElement.html</anchorfile>
      <anchor>af7cde5cfd23cb3b4ca4ddbdbcf717db9</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::TensorFieldParent</name>
    <filename>classcsl_1_1TensorFieldParent.html</filename>
    <base>csl::TensorParent</base>
    <member kind="function">
      <type>cslParent::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1TensorFieldParent.html</anchorfile>
      <anchor>adf100615e12fa1f81d826f0f810e776b</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>cslParent::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1TensorFieldParent.html</anchorfile>
      <anchor>aa21a699e14c7790719ad4ef846f3f584</anchor>
      <arglist>() const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::TensorParent</name>
    <filename>classcsl_1_1TensorParent.html</filename>
    <base>csl::AbstractParent</base>
    <member kind="function">
      <type></type>
      <name>TensorParent</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>ab4af4e561398f75e81b0ca44c2200d8d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorParent</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a04f4e5b6b75716a794e2c275530ae011</anchor>
      <arglist>(const std::string &amp;t_name)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorParent</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a34fe9aa77d890d2f797890d044d23727</anchor>
      <arglist>(const TensorParent &amp;abstract)=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorParent</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a3381ecd1d560c4a80b2bf4804ef45b93</anchor>
      <arglist>(const std::string &amp;t_name, const Space *t_space)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorParent</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a1dea1d8fe995338d286bf35b48396b7b</anchor>
      <arglist>(const std::string &amp;t_name, const std::vector&lt; const Space * &gt; &amp;t_space)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorParent</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a72bf7b821897d7b35f6ede9448c60e43</anchor>
      <arglist>(const std::string &amp;t_name, const std::vector&lt; const Space * &gt; &amp;t_space, const Expr &amp;t_tensor)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TensorParent</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>ada2f60750c2e2d3f7fa573d8de73d313</anchor>
      <arglist>(const std::string &amp;t_name, const Space *t_space, const Expr &amp;t_tensor)</arglist>
    </member>
    <member kind="function">
      <type>cslParent::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a9526dde46fc8cbe38a6cc9910441986f</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>cslParent::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>aa5d0d77665f81cd08e1a053f434daef1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getDim</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a5bff2bf79a3985d9020bee392f440990</anchor>
      <arglist>(const Space *t_space) const override</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; const Space * &gt;</type>
      <name>getSpace</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>ae501e7e880e885ddcd65d946d69ef53c</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Symmetry</type>
      <name>getSymmetry</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a09a777f09a8e69e177beef0055f0062a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getFullySymmetric</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>aa4af33bfd789ef86dd5775b7a4558a01</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getFullyAntiSymmetric</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a4b974963fac5f42fe6f3ea5ca7ed93b6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Permutation &gt;</type>
      <name>getPermutation</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a57d5160b0d1a94e376bbbec3d5ab5735</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getTensor</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a889012a413646fa644b39c24366673f1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getTrace</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a13f343f42228eecf7986e8a361091627</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a7a0dbe923e3491cc35fa48c175baee43</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsExplicitlyOn</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a14f4bcb3c19fb4e8cb8ccf3a61719f27</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFullySymmetric</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>ac99c17db4d68be67761930e0289ee5e0</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFullyAntiSymmetric</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>ae0d6d12f0cd4efe83154cbf79978d088</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addSpace</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a17fb05141415ac442cb8f5b6e88bc329</anchor>
      <arglist>(const Space *space) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addSymmetry</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>ac9a495db8af3074a6c329d84a1b7cb91</anchor>
      <arglist>(int i1, int i2) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addAntiSymmetry</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>abdfaf46f9699cea8ac9cea9b7dcec388</anchor>
      <arglist>(int i1, int i2) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSymmetry</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>aa61f52e3ea9ae4beb190442ac0e7aacc</anchor>
      <arglist>(const Symmetry &amp;t_symetry) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTensor</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a4eacd947decb14e003f81a8053d94e11</anchor>
      <arglist>(const Expr &amp;t_tensor) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setElementary</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a19c75060fbf2040abeb2ca7c61069aec</anchor>
      <arglist>(bool t_elementary)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasContractionProperty</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>aee4452e0bd10be255d394ed3463780a3</anchor>
      <arglist>(const Abstract *self, Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>contraction</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>afdb6b8332cd4a18e3835099e0fab9f32</anchor>
      <arglist>(const Abstract *self, Expr_info B) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addSelfContraction</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a258388f5044e36fa76825710e67ebada</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, const Expr &amp;res, std::optional&lt; std::function&lt; bool(Expr_info, Expr_info)&gt;&gt; condition=std::nullopt) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>operator()</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a335867b85b438d48eaf0457cb5c83469</anchor>
      <arglist>(Index index) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual Expr</type>
      <name>operator()</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a3cbe8233fd122bd6f4f3b778baa47df4</anchor>
      <arglist>(std::vector&lt; Index &gt; indices) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a18f7711d34c8bcdb3e034b7b82b76e09</anchor>
      <arglist>(const TensorParent &amp;other) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>operator!=</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>acb363ae0d9495875816d243352735ccf</anchor>
      <arglist>(const TensorParent &amp;other) const</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::vector&lt; const Space * &gt;</type>
      <name>space</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>ae740d077dfcc386a407ca33d3619a5a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Symmetry</type>
      <name>symmetry</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>adc7dcdd4a866af29f99edc06924c2617</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>fullySymmetric</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a59e24724c7c0c2ee05617769c78c454e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>fullyAntiSymmetric</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>aa0dae8fe45934513b287849169dbba93</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::vector&lt; std::pair&lt; SelfContraction, Expr &gt; &gt;</type>
      <name>selfContraction</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a10cab17d2ba7b927d3b30278201ea9dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>valued</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>ac374a59f2a77225b51fd4f01f727f73b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Expr</type>
      <name>tensor</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a61267f78d93f470771cf63f174322e01</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Expr</type>
      <name>trace</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>a5db8129d9af05c907e28f046471650e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classcsl_1_1TensorParent.html</anchorfile>
      <anchor>aaf4be36622045a04864371bceec13d8a</anchor>
      <arglist>(std::ostream &amp;fout, const TensorParent &amp;)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>csl::TimeMonitor::Time</name>
    <filename>structcsl_1_1TimeMonitor_1_1Time.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::TimeMonitor</name>
    <filename>classcsl_1_1TimeMonitor.html</filename>
    <class kind="struct">csl::TimeMonitor::Time</class>
  </compound>
  <compound kind="class">
    <name>csl::Timer</name>
    <filename>classcsl_1_1Timer.html</filename>
  </compound>
  <compound kind="struct">
    <name>csl::matcher::Tree</name>
    <filename>structcsl_1_1matcher_1_1Tree.html</filename>
  </compound>
  <compound kind="class">
    <name>csl::Variable</name>
    <filename>classcsl_1_1Variable.html</filename>
    <base>csl::AbstractLiteral</base>
    <member kind="function">
      <type></type>
      <name>Variable</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a42615d2ddcded377aad952c5b90e1a8b</anchor>
      <arglist>(const std::shared_ptr&lt; VariableParent &gt; &amp;t_parent)</arglist>
    </member>
    <member kind="function">
      <type>csl::PrimaryType</type>
      <name>getPrimaryType</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a1b67145a27e8b33488f3243a97ce2064</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>acbc90bf0cbc0c78553b26fbfa7eb207a</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::string const  &amp;</type>
      <name>getName</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a7ebe3e5b8b4ce47890e7ca4bbf6d634e</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getCommutable</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>ab418e2b424462798f44ebfe186b445c3</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setName</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a48c1ccf9f8ff0fa0b533f7e986340b85</anchor>
      <arglist>(const std::string &amp;name) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCommutable</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>ac51328102c02c7f24ace7742db02537c</anchor>
      <arglist>(bool commutable) override</arglist>
    </member>
    <member kind="function">
      <type>Parent</type>
      <name>getParent</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a3eea13fd67db0b8f0b5accea6adefb21</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getValued</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>ad0ba237abda5d2a01a50a36a5daf6248</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>getValue</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>aed2e30529caec691269a11608b678e41</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>aa8f5dda90b3f915c633745740e579abd</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>commutesWith</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>adfa4fb552a7ff6ab32eeea0a39dc97cd</anchor>
      <arglist>(Expr_info expr, int sign=-1) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>acc912f9063706e9222537df9f2035ed1</anchor>
      <arglist>(long double t_value) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a67c31bd17fd8671bf460b6f164ed4ca7</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a595fc7eb8f15fe01c80903a7fea41890</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>evaluateScalar</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a07be9df0d692e8a4dcd7ac913baee1ac</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>evaluate</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a96c4549dce7d30db1f06180f8d3d0145</anchor>
      <arglist>(csl::eval::mode user_mode=csl::eval::base) const override</arglist>
    </member>
    <member kind="function">
      <type>std::optional&lt; Expr &gt;</type>
      <name>derive</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a22efd34dbcb5934a5acaf95efeef91b3</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getParity</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>a12415e4c7a0296803e0b4fb6a174ac4b</anchor>
      <arglist>(Expr_info t_variable) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator=</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>abf8caba816e7baa5bcea4961187bd0c6</anchor>
      <arglist>(double t_value) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1Variable.html</anchorfile>
      <anchor>abb165be8425c313e0532831604e11d5f</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::VariableParent</name>
    <filename>classcsl_1_1VariableParent.html</filename>
    <base>csl::LiteralParent</base>
    <member kind="function">
      <type>cslParent::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1VariableParent.html</anchorfile>
      <anchor>aba33fb8ddc1cc378ac6178c6aaee6e7a</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dependsOn</name>
      <anchorfile>classcsl_1_1VariableParent.html</anchorfile>
      <anchor>aa752d83ccf00b8277329367836af6488</anchor>
      <arglist>(Expr_info expr) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::Vector</name>
    <filename>classcsl_1_1Vector.html</filename>
    <base>csl::AbstractVectorial</base>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1Vector.html</anchorfile>
      <anchor>a2b3aef310427653b07c7963a606faca8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getSubVectorial</name>
      <anchorfile>classcsl_1_1Vector.html</anchorfile>
      <anchor>a8a948bfe3dad300352a8bde32679cb82</anchor>
      <arglist>(int iExcept) const override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>csl::VectorIntegral</name>
    <filename>classcsl_1_1VectorIntegral.html</filename>
    <base>csl::AbstractIntegral</base>
    <member kind="function">
      <type></type>
      <name>~VectorIntegral</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>af928caed3db0a3e8f5042fdf1a8baf21</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>csl::Type</type>
      <name>getType</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>a514f7d0f5a9cff6c43d786717a030ad7</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>getVariable</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>a0bdc27a5f2a2533af9aaf96f4582ed07</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>Parent</type>
      <name>getParent</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>a956aaa1428471ce72c9c273cd5ce4ac8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>a208b56fddd7449cbcd3bb85ceca972f6</anchor>
      <arglist>(int mode=0, std::ostream &amp;out=std::cout, LibraryMode libMode=LibraryMode::NoLib) const override</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>printLaTeX</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>a8d48fe368eea528433f18e5d8eb7c3a8</anchor>
      <arglist>(int mode=0) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isIndexed</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>ab3961d7313002bd6e4d0c342bd2cd9a1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>IndexStructure</type>
      <name>getIndexStructure</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>a2b23eb2012ae1b985e0e2318f11cf3f5</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>a2e912283405785f512234ada336ca5dd</anchor>
      <arglist>(Expr_info other) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareWithDummy</name>
      <anchorfile>classcsl_1_1VectorIntegral.html</anchorfile>
      <anchor>a6a1e720e6db9d81db825d18cea8a334a</anchor>
      <arglist>(Expr_info expr, std::map&lt; Index, Index &gt; &amp;constraints, bool keepAllCosntraints=false) const override</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>csl</name>
    <filename>namespacecsl.html</filename>
    <class kind="class">csl::Abbreviation</class>
    <class kind="class">csl::Abbrev</class>
    <class kind="class">csl::Abstract</class>
    <class kind="class">csl::Expr</class>
    <class kind="class">csl::BooleanOperator</class>
    <class kind="class">csl::AbstractBuildingBlock</class>
    <class kind="class">csl::Complexified</class>
    <class kind="class">csl::Commutator</class>
    <class kind="class">csl::Arbitrary</class>
    <class kind="class">csl::Comparator</class>
    <class kind="class">csl::OptionalCondition</class>
    <class kind="class">csl::RealPart</class>
    <class kind="class">csl::ImaginaryPart</class>
    <class kind="class">csl::Diagonalizer</class>
    <class kind="class">csl::AbstractElement</class>
    <class kind="class">csl::Functional</class>
    <class kind="class">csl::Index</class>
    <class kind="class">csl::IndexStructure</class>
    <class kind="class">csl::IndexStructure_new</class>
    <class kind="class">csl::IndexStructureView</class>
    <class kind="class">csl::SelfContraction</class>
    <class kind="class">csl::ContractionChain</class>
    <class kind="class">csl::TensorParent</class>
    <class kind="class">csl::EpsilonParent</class>
    <class kind="class">csl::DeltaParent</class>
    <class kind="class">csl::MetricParent</class>
    <class kind="class">csl::Tensor</class>
    <class kind="class">csl::TensorElement</class>
    <class kind="class">csl::ISum</class>
    <class kind="class">csl::IProd</class>
    <class kind="class">csl::InitSanitizer</class>
    <class kind="class">csl::LibDependency</class>
    <class kind="class">csl::LibEval</class>
    <class kind="class">csl::LibEvalSession</class>
    <class kind="struct">csl::LibParameter</class>
    <class kind="class">csl::LibFunction</class>
    <class kind="class">csl::LibraryGenerator</class>
    <class kind="class">csl::LibraryGroup</class>
    <class kind="class">csl::linear_map</class>
    <class kind="class">csl::AbstractLiteral</class>
    <class kind="class">csl::Constant</class>
    <class kind="class">csl::Variable</class>
    <class kind="class">csl::IntFactorial</class>
    <class kind="class">csl::Imaginary</class>
    <class kind="class">csl::Lock</class>
    <class kind="class">csl::Abs</class>
    <class kind="class">csl::Exp</class>
    <class kind="class">csl::Log</class>
    <class kind="class">csl::Cos</class>
    <class kind="class">csl::Sin</class>
    <class kind="class">csl::Tan</class>
    <class kind="class">csl::ACos</class>
    <class kind="class">csl::ASin</class>
    <class kind="class">csl::ATan</class>
    <class kind="class">csl::Angle</class>
    <class kind="class">csl::Cosh</class>
    <class kind="class">csl::Sinh</class>
    <class kind="class">csl::Tanh</class>
    <class kind="class">csl::ACosh</class>
    <class kind="class">csl::ASinh</class>
    <class kind="class">csl::ATanh</class>
    <class kind="class">csl::Factorial</class>
    <class kind="class">csl::DiracDelta</class>
    <class kind="class">csl::MultiPermutation</class>
    <class kind="class">csl::AbstractNumerical</class>
    <class kind="class">csl::Integer</class>
    <class kind="class">csl::Float</class>
    <class kind="class">csl::IntFraction</class>
    <class kind="class">csl::Complex</class>
    <class kind="class">csl::ObjectSymmetry</class>
    <class kind="class">csl::ObjectPermutation</class>
    <class kind="class">csl::Sum</class>
    <class kind="class">csl::Polynomial</class>
    <class kind="class">csl::Prod</class>
    <class kind="class">csl::Pow</class>
    <class kind="class">csl::Derivative</class>
    <class kind="class">csl::Integral</class>
    <class kind="class">csl::Operator</class>
    <class kind="class">csl::AbstractParent</class>
    <class kind="class">csl::Parent</class>
    <class kind="struct">csl::ExpansionChain</class>
    <class kind="class">csl::ProgressBar</class>
    <class kind="class">csl::PseudoIntegral</class>
    <class kind="class">csl::AbstractIntegral</class>
    <class kind="class">csl::ScalarIntegral</class>
    <class kind="class">csl::VectorIntegral</class>
    <class kind="struct">csl::canDecay</class>
    <class kind="class">csl::AbstractFunc</class>
    <class kind="class">csl::AbstractDuoFunc</class>
    <class kind="class">csl::AbstractMultiFunc</class>
    <class kind="class">csl::ScopedProperty</class>
    <class kind="class">csl::Space</class>
    <class kind="class">csl::Permutation</class>
    <class kind="class">csl::Symmetry</class>
    <class kind="class">csl::IndexedSymmetry</class>
    <class kind="class">csl::TensorFieldParent</class>
    <class kind="class">csl::TensorField</class>
    <class kind="class">csl::TensorFieldElement</class>
    <class kind="class">csl::TDerivativeElement</class>
    <class kind="class">csl::TDerivativeParent</class>
    <class kind="class">csl::TDerivative</class>
    <class kind="class">csl::TimeMonitor</class>
    <class kind="class">csl::Timer</class>
    <class kind="class">csl::LiteralParent</class>
    <class kind="class">csl::ConstantParent</class>
    <class kind="class">csl::VariableParent</class>
    <class kind="class">csl::AbstractVectorial</class>
    <class kind="class">csl::Vector</class>
    <class kind="class">csl::Matrix</class>
    <class kind="class">csl::HighDTensor</class>
    <member kind="typedef">
      <type>std::function&lt; std::optional&lt; csl::Expr &gt;(csl::Expr const &amp;, bool)&gt;</type>
      <name>replacementRule</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6e8b412297fea1afcb1cbbce97556e08</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>PrimaryType</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a70046459ec0082db8cb97005c13c9a54</anchor>
      <arglist></arglist>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a3c1fc46575f79b15fc8990642387d07f">Arbitrary</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a5225763efc97aebd39e5a303fceb57ac">Numerical</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54ad7da1b76e5799f53a399b7a96ba67437">Literal</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a78216444cbb3388cc8c2f64cc44c7cd5">MultiFunction</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a8870e734b769add08821a829f6951853">ScalarFunction</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a97a447e7de95e9e2940291e17bcc4e3a">Vectorial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a25e27f9899952c474587a6bee3ab9978">Indicial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="a70046459ec0082db8cb97005c13c9a54a6f16a5f8ff5d75ab84c018adacdfcbb7">Field</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ace591865c69b6315c533a1a325777312</anchor>
      <arglist></arglist>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a892b1f4fc1fcdaaf7dfed08ced04a389">InheritanceType</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a9080fd7a5f0af44b585bb21b62d8b283">NoType</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312aa0faef0851b4294c06f2b94bb1cb2044">Integer</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a22ae0e2b89e5e3d477f988cc36d3272b">Float</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312aa9e4acf6d1d381d6875f480a5737b8b4">IntFraction</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312aa79046cb7e9f609ba07c4ec22f14a862">IntFactorial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a10b4eb76294b70d7fd6df997ff06edb1">Complex</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ab87c58a6946efb3fd56cc53586d2fb78">NumericalEval</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af19f497d4c860d252cc1e055d0362ccc">Imaginary</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312acb17869fe51048b5a5c4c6106551a255">Constant</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a47c14840d8e15331fa420b9b2f757cd9">Variable</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312acad39a154bffb61175f674d6eefaf6d0">Exp</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ace0be71e33226e4c1db2bcea5959f16b">Log</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a29524358ad4c2579d7b08a21c990ce5e">DiracDelta</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a1e34af023adeb7d5f484f8eb4b9826b6">Abs</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312affca562be079b9e4e41ea9d6a86c582b">Cos</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a0986d137604183312e6d3599578bc6cd">Sin</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a1266b4e6f81e60733ec6c717e0181f60">Tan</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a4371c3565b771b40c187487cc0190392">Cosh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a98ed804abf4edd1f3be3fa894fa8cc3d">Sinh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312acc132a41cab5676334f353a22a0aa5c5">Tanh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a1c3ca8e525c0e6a47cc8489cb3155365">ACos</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a118d7e2878ba8789c8f86eee41c470b3">ASin</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af9be9640e98f6072e8e0eaf2b418efe2">ATan</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a27cebb75cb5855484f7df7070b4cab73">ACosh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a6964cd88a53b2e4c358bcad0d9feac55">ASinh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a2286f4e717c6213c895da49a51aa5de7">ATanh</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af60357a8d17e45793298323f1b372a74">Scalar</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ac74cc04b1519df2caf5026e94c3e8bab">RealPart</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ac49d71b3e882559a62d9d17009728907">ImaginaryPart</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a84f9e7bc59dd338537142e95bbd5521a">BooleanOperator</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a2ed2c32254f39662881b6eed72021250">TensorElement</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a02e48a623b58c0b63bd2ad0e891c944e">ScalarField</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a5a44d898396f28619971f227d438ef40">TensorFieldElement</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a9134ead1dfb798c5efccfaf181501eeb">TDerivativeElement</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af575ff69fb3dc1eeb3341d4a419cada2">StandardDuo</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a607547b66dcd8e516cde5cc78739ae80">StandardMult</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a8be6d3e955290ee50c00a4bc92aa7749">Pow</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a362a33c23b08e4a32a4ec53fbb82cccd">Prod</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312aa0ec87054b5e5b7847d0d8780a01a3d5">Sum</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a24ecfbe376a82f09ad48bffc6b8d6a87">Polynomial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a70ae6e285cc14c8486e3cf5bec39d1fd">Derivative</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312acc1e31375d9c49eb15a3302b96acc061">Commutator</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a45f4ce6c3306644b1efe333f4f8d6929">Angle</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a012b11265b94945f7a1390a07776e625">Factorial</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a4ea94552a2bec56a29592359a1b6069e">Integral</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312ad4d8fc83b4874f82d0b698683c6b9e22">ScalarIntegral</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a7a5b7a2e7ef67739db0cf0cc4d3e5d60">VectorIntegral</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a57dea6f5039281b7fee517fc43bf3110">Vector</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312af53df0293e169f562bc1d9a20e1d2589">Matrix</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ace591865c69b6315c533a1a325777312a93ff2aa452bc6cbb7c3fe56cc7d196c5">HighDTensor</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ComplexProperty</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac54d9e79dc6697a8eacc07c754b11c0e</anchor>
      <arglist></arglist>
      <enumvalue file="namespacecsl.html" anchor="ac54d9e79dc6697a8eacc07c754b11c0ea10b4eb76294b70d7fd6df997ff06edb1">Complex</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ac54d9e79dc6697a8eacc07c754b11c0ea7f80fcc452c2f1ed2bb51b39d0864df1">Real</enumvalue>
      <enumvalue file="namespacecsl.html" anchor="ac54d9e79dc6697a8eacc07c754b11c0eaf19f497d4c860d252cc1e055d0362ccc">Imaginary</enumvalue>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator+</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a4214d96bf84e933c268cf30c85ef5065</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator-</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8c85cf9cc27a2f9ea69c28fed0243a66</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator*</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a51da3cbce74c766bb1aff63488113698</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator/</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a75e84d94b9caf4662aa30a5c6aa36a63</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac52343b89158d08dc27aa4851ba1662c</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a73fc26b671ec2e0fa84ba12983ed9866</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a64388fa8d8a566d1b1b565f888acfaa5</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a612503ed45700a00e76eccfd796bfec1</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abdcf5895808d59f7113b9c9dc28133ca</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6dbe8e8ea5d3a9880c2d95741728f8d1</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>minus_</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aa1186b8efe45b2ea34ffcf8e0de69238</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>prod_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae8a7809aa1495de26d449b25c5676215</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, bool explicitProd=0)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>fraction_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>adb8fb620d09d80258c0c3d0c2a72340a</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>pow_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a39a4d5d0b5daeb571db05ae3bb2d36e4</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>derivative_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a34118703c2961443732bb747369f30ac</anchor>
      <arglist>(const Expr &amp;leftOperand, const Expr &amp;rightOperand, int order=1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator|=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a187485362620c294cc6cf77f797d07a7</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&amp;=</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae5f3a8cb78aea12f4d966effd040681b</anchor>
      <arglist>(const Expr &amp;a, const Expr &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>FindLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae25492ddb894bb646ca12868bbd5e54d</anchor>
      <arglist>(Expr const &amp;init, Expr const &amp;value, size_t depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>FindNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a16030249dd9a145214dad11c6c698c75</anchor>
      <arglist>(Expr const &amp;init, Expr const &amp;node, size_t depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>FindIfLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a27562270e1babd293920469862804ddf</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, size_t depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>FindIfNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af08b277cdb6f9d098af7640035563e5d</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, size_t depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>AnyOfLeafs</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a0c725982c603dc7243b94238c7399f17</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>AllOfLeafs</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab34e46d9db1bf2c52a11e16edb96aac9</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>AnyOfNodes</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aee4fb7f5bf59e525f167e3e5e1bea6ed</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>AllOfNodes</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a2a09c25d95caf5531f593f5e8a92cc99</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>VisitEachNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab32d8a121c108b533a4e746a10d737ba</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; void(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>VisitEachLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a795b923fd042a73265a04f5fa551362b</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; void(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>VisitEachNodeCut</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abfb7fdd6c18bd8a98bfa3674e4bebefb</anchor>
      <arglist>(Expr const &amp;init, std::function&lt; bool(Expr const &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ForEachNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8bbe21d8979e1d4fa2e9242a34c397be</anchor>
      <arglist>(Expr &amp;init, std::function&lt; void(Expr &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ForEachNodeCut</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af1844d0ef7e9f50ccd490096c4f1ff21</anchor>
      <arglist>(Expr &amp;init, std::function&lt; bool(Expr &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ForEachLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af74bfbc9faa2de7d49f5d37403795b73</anchor>
      <arglist>(Expr &amp;init, std::function&lt; void(Expr &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>FirstOfNode</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abe86be4e2295c35823a965faf12bb0c7</anchor>
      <arglist>(Expr &amp;init, std::function&lt; bool(Expr &amp;)&gt; const &amp;f)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>FirstOfLeaf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af9f72de45510b266cb259a69d11cbf14</anchor>
      <arglist>(Expr &amp;init, std::function&lt; bool(Expr &amp;)&gt; const &amp;f)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>Transform</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aba0af68b54eefa2d567faed146678ad7</anchor>
      <arglist>(Expr &amp;init, std::function&lt; bool(Expr &amp;)&gt; const &amp;f, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Commutation</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a15620d82e51a1ba2c082a0b17ed773d0</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, int sign=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>WeakCommutation</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab82c5a52247dd5ef84518d51deade500</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, int sign=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>commutator_</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>afa314ac9077b2d18a81fa954327ab7e0</anchor>
      <arglist>(const Expr &amp;A, const Expr &amp;B, int t_sign=-1)</arglist>
    </member>
    <member kind="function">
      <type>Iterator</type>
      <name>dichotomyFindIf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae69cdb1c6ddfa666c766cd2edf66823e</anchor>
      <arglist>(Iterator first, Iterator last, Comparator &amp;&amp;f)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad68e55a1e260433cbafab69addbb53c4</anchor>
      <arglist>(std::ostream &amp;fout, csl::Type type)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a073cd06df1b6c4e0efb497d67d87e275</anchor>
      <arglist>(std::ostream &amp;fout, csl::PrimaryType primaryType)</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>operator!</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a7239c266a7391f8a652b83e65d927f4a</anchor>
      <arglist>(const Index &amp;index)</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>operator+</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae8258f8706933e1c10c973c721cd6151</anchor>
      <arglist>(const Index &amp;index)</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>operator-</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aa060a7f29c77b51682733901c7a24d04</anchor>
      <arglist>(const Index &amp;index)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab115b83e97fbec6f2ac954e5ad21aa19</anchor>
      <arglist>(std::ostream &amp;fout, const TensorParent &amp;i)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>nameTensor</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab17fb7de179f8643818e2eb85c4adda8</anchor>
      <arglist>(const std::string &amp;name, Expr &amp;tensor, bool first=true)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>generateTensor</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6343f864651a51563696008ea8c73f64</anchor>
      <arglist>(const std::string &amp;name, const std::vector&lt; const Space * &gt; &amp;spaces)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ApplySelfProperty</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>acf713387a07a13348155f7043b55c933</anchor>
      <arglist>(Expr &amp;init, csl::Tensor &amp;tensor, Expr const &amp;A, Expr const &amp;B, Expr const &amp;res)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ApplyChainProperty</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a36d313d19ed9c1b6e4cfbefbb7af7169</anchor>
      <arglist>(Expr &amp;init, csl::Tensor &amp;tensor, std::vector&lt; Expr &gt; const &amp;prod, Expr const &amp;res)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>CheckValidity</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a9b1baa69edc83c2d90efd6f79bbdb0b4</anchor>
      <arglist>(Expr const &amp;init, std::vector&lt; Expr_info &gt; encountered=std::vector&lt; Expr_info &gt;())</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>MemorySizeOf</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a769c039db45134ce7cbdd918da92273b</anchor>
      <arglist>(Expr const &amp;expression)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>intfactorial_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a7fbc14af6671615ab035d1c56fdcc02a</anchor>
      <arglist>(int value)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>abs_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a5327d368e944125c5ffa58b304051751</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>exp_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a677874110c8d7d3febc7f30085273e6b</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>log_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a77a31d3871e8136ab04e15368528c846</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>cos_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a3033bff78373155994cbf45ec3e71493</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>sin_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a5f030b62fc81f11147493796bfa30b54</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>tan_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>afbaab772a8fb873c65e06d079e73cd06</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>acos_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a51f0625e0729c8516296eaa4e82a9928</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>asin_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac5fe6fa63f0b6cd80fd9edd625a715ad</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>atan_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a55a28054aa1541d982b05e4c90e22243</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>cosh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a9f96bc23faf2e85278c67d2e77b02430</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>sinh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a2aab7af831ac14498b23ac3daa80d1d5</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>tanh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8a0975103bcda47f0e7a335677ba8e1f</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>acosh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a832f3758b9ec7e3f52e554b949c8acf3</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>asinh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6f8d17a63615e912a201b11ce163b9dd</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>atanh_s</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac7d65507d0aa73726a4f0969ba1134be</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a951a9c5e68399460d980c205f744a1a9</anchor>
      <arglist>(std::ostream &amp;fout, const AbstractParent &amp;parent)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasWeakDependency</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a65b6337073b66a62d69827fd9246f092</anchor>
      <arglist>(csl::Expr const &amp;expr, std::function&lt; bool(csl::Expr const &amp;)&gt; const &amp;predicate)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyThroughAbbreviations</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a178cb5ff11ecfb69f32bbdea3253712a</anchor>
      <arglist>(csl::Expr &amp;expr, replacementRule const &amp;rule)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>ruleToPredicate</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abc696cee24297cc1ca72e9ce5cfae806</anchor>
      <arglist>(replacementRule const &amp;rule)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>printVector</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a4dc45c94957fc5b977345f3710daa971</anchor>
      <arglist>(const csl::vector_expr &amp;vector)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addAlternateForm</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a75d23a79a366d8af5519ca40795d1d98</anchor>
      <arglist>(csl::vector_expr &amp;alternateForms, const Expr &amp;newAlternate, bool add_factor_expand=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reduceAlternate</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>abbdbabfc34997aff95b6a271afa8de2f</anchor>
      <arglist>(csl::vector_expr &amp;alternateForms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearRedundancyAlternate</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ac7fe69183e8550e7fce8019a409bde81</anchor>
      <arglist>(csl::vector_expr &amp;alternateForms)</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr</type>
      <name>getRecursiveAlternateForms</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad3617f2bee57ef5bf099950c54160a4e</anchor>
      <arglist>(const Expr &amp;expr, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>csl::vector_expr</type>
      <name>internalRecursiveAlternateForms</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8756d45335e213a1323b7ee840134e18</anchor>
      <arglist>(const Expr &amp;expr, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Simplify</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad27c8d3dd60c235e3e804b97790110ab</anchor>
      <arglist>(const Expr &amp;expr, int depth=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>selectionSort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>adcde11ad745cfee42e9efa83d145031a</anchor>
      <arglist>(std::vector&lt; Expr &gt; &amp;argument)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a30af37876f8c6102a9a0ffcb83c78f13</anchor>
      <arglist>(std::vector&lt; Expr &gt; &amp;argument)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mergeSort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a1c48c85e78f6b663a82bd7e100ad68d0</anchor>
      <arglist>(std::vector&lt; Expr &gt; &amp;argument)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a038131f4648d3a85d7f66956b9a052f9</anchor>
      <arglist>(std::vector&lt; Expr &gt;::iterator first, std::vector&lt; Expr &gt;::iterator last)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>selectionSort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aaa60374eb375314f6f7c661a35c01e67</anchor>
      <arglist>(std::vector&lt; Expr &gt;::iterator first, std::vector&lt; Expr &gt;::iterator last)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mergeSort</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af4d5f0bc5267d581906fe5a40f3dcce1</anchor>
      <arglist>(std::vector&lt; Expr &gt;::iterator first, std::vector&lt; Expr &gt;::iterator last)</arglist>
    </member>
    <member kind="function">
      <type>const Space</type>
      <name>Euclid_R2</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>adc609bb5307e2707f433fd90847da159</anchor>
      <arglist>(&quot;R2&quot;, 2)</arglist>
    </member>
    <member kind="function">
      <type>const Space</type>
      <name>Euclid_R3</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae2032bf9b522dae416eb0c7f66642e89</anchor>
      <arglist>(&quot;R3&quot;, 3)</arglist>
    </member>
    <member kind="function">
      <type>const Space</type>
      <name>Euclid_R4</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8d1d8eb1cfa212c44e0ef199e78db957</anchor>
      <arglist>(&quot;R4&quot;, 4)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>PGCD</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a8c9bbef2158b7cb97a7d45e8739e01d5</anchor>
      <arglist>(double a, double b)</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>PGCD</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a293f777248d9b27ff737fef8d51bb544</anchor>
      <arglist>(long long int a, long long int b)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>internal_PGCD</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a776ff8b234198c45db2d6233508ad558</anchor>
      <arglist>(long long int a, long long int b)</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>sgn</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a62ae14b0bc1a6b3f7e9138421b2b8360</anchor>
      <arglist>(long long int a)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>sgn</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a36cdda72e72136f4b0afec1d865361c5</anchor>
      <arglist>(double a)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>factorial</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a5bd20a50cf5cdfbe57e3b896b980179e</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>long long int</type>
      <name>internal_factorial</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a88fe2378efd87d7bc05ba13dbb35fa9e</anchor>
      <arglist>(long long int n)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>comparePlaceIndependant</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a31f687274fba7d8f4dc865a21a1901ac</anchor>
      <arglist>(std::vector&lt; T &gt; A, std::vector&lt; T &gt; B)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>partialComparePlaceIndependant</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad90b89dc5d8450e4ce9538431f80925b</anchor>
      <arglist>(std::vector&lt; T &gt; A, std::vector&lt; T &gt; B)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a7fabac6b19ca3a7a796ad43c42a66e32</anchor>
      <arglist>(std::ostream &amp;fout, const Permutation &amp;permutation)</arglist>
    </member>
    <member kind="function">
      <type>vector&lt; Permutation &gt;</type>
      <name>permutations</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a35c72686b5438eec934374e4d151e4fc</anchor>
      <arglist>(const Permutation &amp;init)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reducePermutation</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad292d5ba73b91dee12de72e4c96996b3</anchor>
      <arglist>(std::vector&lt; Permutation &gt; &amp;permutation)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Permutation &gt;</type>
      <name>getSpan</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a6fe98d1ad16fa3c08f8a35317438fcb1</anchor>
      <arglist>(const std::vector&lt; Permutation &gt; &amp;init)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>getSpan</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a67a9246a8bb37b33f7117f1e1eb571f9</anchor>
      <arglist>(std::vector&lt; Permutation &gt; &amp;spanned, const Permutation &amp;element)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a1e37af5218572ce22681d13fec4e4eaa</anchor>
      <arglist>(std::ostream &amp;fout, const Symmetry &amp;symmetry)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; int &gt; &gt;</type>
      <name>permutations</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ad16a083cc7c50de69d67b66c20e030f2</anchor>
      <arglist>(std::vector&lt; int &gt; init)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Copy</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab7420e1ff6f6a7d9f069bca1c6ac16f5</anchor>
      <arglist>(const Abstract *expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Copy</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab074eed4bd06ee44010c91e2d1dd05cb</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>DeepCopy</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>af1e310aad90377d99f591def8b30336c</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>DeepCopy</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a40f0cf1575083e98534941e8aee6cfad</anchor>
      <arglist>(const Abstract *expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Refreshed</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a9c5e3c2f5f4c6533ac74d150d29f931d</anchor>
      <arglist>(const Abstract *expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>Refreshed</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a90a30db275050c669fbe24152402c28e</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>DeepRefreshed</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ae95bf5940e1dda061d2d9fbdd1a9fff8</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>SIMPLIFICATION_METHOD</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a87993eddca471f11b91e56d071675420</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>MAX_ALTERNATE_FORMS</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>a4cfdfd259da7ff4d06994eed7d4b99ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>MAX_RECURSION_ALTERNATE</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>adbdaaf75e2d3ed39cc0e33633d2300dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>minMergeSize</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>ab3885c426f3cbbc274fe4ce33aae739b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const Space &amp;</type>
      <name>Minkowski</name>
      <anchorfile>namespacecsl.html</anchorfile>
      <anchor>aef525ce7226ae5fa0fc9b9f2aa4ccc5d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cslIndex</name>
    <filename>namespacecslIndex.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>ad333cdff46bc7adff9474bfb9d9d3f4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Free</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>ad333cdff46bc7adff9474bfb9d9d3f4eaf6c415d974f23a8d98634cd1e91f6ad9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Dummy</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>ad333cdff46bc7adff9474bfb9d9d3f4ea07288b108ee7f23274ef7af2fc1df9c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Fixed</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>ad333cdff46bc7adff9474bfb9d9d3f4eaea7fa93a248dbab81642463f10ba55cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>TensorType</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>aa49df84371c2616835b29cb0e5733abb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Metric</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>aa49df84371c2616835b29cb0e5733abba8f3767576db7f62a2864244c8e2841b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Delta</name>
      <anchorfile>namespacecslIndex.html</anchorfile>
      <anchor>aa49df84371c2616835b29cb0e5733abba56812cd3f023e8a71d7dc6ad3904ac7d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>cslParent</name>
    <filename>namespacecslParent.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>PrimaryType</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a83f078a60370b86ccd6b113eedff51c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Indicial</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a83f078a60370b86ccd6b113eedff51c2ac11c4f92f20580eb0d84f14ec49e41ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Field</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a83f078a60370b86ccd6b113eedff51c2a578739351cbc014d9a0b31850b6899a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>GenericIndicial</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339aa84cb71d316a2d84edc0717cb4ca0efc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Metric</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339ad30e0f678eff5f93c8bb23ff181de4d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Delta</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339aa6b88b45ff88168338ab570f6c640d53</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ScalarField</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339acaa17b0c5fa8433f006fb1823931dc17</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TensorFieldElement</name>
      <anchorfile>namespacecslParent.html</anchorfile>
      <anchor>a880f05263e57bd6cac32e48fd45b7339ab3dc2eea537b69b0d85acc45dbc548a1</anchor>
      <arglist></arglist>
    </member>
  </compound>
</tagfile>
